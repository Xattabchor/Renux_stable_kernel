#!/usr/bin/env python3
"""Bake Quantum-style RGB vertex illumination into an OBJ mesh.

The output uses the common OBJ vertex-colour extension:
    v x y z r g b
The RGB values are linear illumination, not material albedo. Import them into
vertex.color for DynamicBakedShotFlashLighting.vp. This is an offline,
static-light baker; runtime shot flashes and flashlights stay in the VGP.

The baker supports direct sky/ground ambient, a shadowed directional sun,
shadowed point lights, cosine-sampled ambient occlusion, and configurable
multi-bounce diffuse path tracing. It reads constant Kd/Ke values from OBJ
material libraries; texture maps are not sampled.
"""
from __future__ import annotations

import argparse
import math
import os
import random
import shlex
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable, Optional

Vec = tuple[float, float, float]


def add(a: Vec, b: Vec) -> Vec:
    return (a[0] + b[0], a[1] + b[1], a[2] + b[2])


def sub(a: Vec, b: Vec) -> Vec:
    return (a[0] - b[0], a[1] - b[1], a[2] - b[2])


def mul(a: Vec, s: float) -> Vec:
    return (a[0] * s, a[1] * s, a[2] * s)


def hadamard(a: Vec, b: Vec) -> Vec:
    return (a[0] * b[0], a[1] * b[1], a[2] * b[2])


def dot(a: Vec, b: Vec) -> float:
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2]


def cross(a: Vec, b: Vec) -> Vec:
    return (a[1] * b[2] - a[2] * b[1],
            a[2] * b[0] - a[0] * b[2],
            a[0] * b[1] - a[1] * b[0])


def length(a: Vec) -> float:
    return math.sqrt(dot(a, a))


def normalize(a: Vec) -> Vec:
    n = length(a)
    if n < 1.0e-12:
        return (0.0, 1.0, 0.0)
    return (a[0] / n, a[1] / n, a[2] / n)


def clamp01(x: float) -> float:
    return max(0.0, min(1.0, x))


@dataclass(frozen=True)
class Triangle:
    a: Vec
    b: Vec
    c: Vec
    normal: Vec
    bounds_min: Vec
    bounds_max: Vec
    centroid: Vec
    albedo: Vec
    emission: Vec

    @classmethod
    def make(cls, a: Vec, b: Vec, c: Vec, albedo: Vec, emission: Vec) -> "Triangle":
        n = normalize(cross(sub(b, a), sub(c, a)))
        return cls(a, b, c, n,
                   tuple(min(a[i], b[i], c[i]) for i in range(3)),
                   tuple(max(a[i], b[i], c[i]) for i in range(3)),
                   tuple((a[i] + b[i] + c[i]) / 3.0 for i in range(3)),
                   albedo, emission)  # type: ignore[arg-type]

    def intersect(self, origin: Vec, direction: Vec, max_t: float) -> Optional[float]:
        # Moller-Trumbore; two-sided so open and inconsistently-wound OBJ meshes
        # still occlude rays.
        e1 = sub(self.b, self.a)
        e2 = sub(self.c, self.a)
        p = cross(direction, e2)
        det = dot(e1, p)
        if abs(det) < 1.0e-10:
            return None
        inv_det = 1.0 / det
        tvec = sub(origin, self.a)
        u = dot(tvec, p) * inv_det
        if u < 0.0 or u > 1.0:
            return None
        q = cross(tvec, e1)
        v = dot(direction, q) * inv_det
        if v < 0.0 or u + v > 1.0:
            return None
        t = dot(e2, q) * inv_det
        return t if 1.0e-5 < t < max_t else None


@dataclass
class BVHNode:
    lo: Vec
    hi: Vec
    triangles: Optional[list[Triangle]] = None
    left: Optional["BVHNode"] = None
    right: Optional["BVHNode"] = None

    @classmethod
    def build(cls, triangles: list[Triangle], leaf_size: int = 8) -> "BVHNode":
        lo = tuple(min(t.bounds_min[i] for t in triangles) for i in range(3))
        hi = tuple(max(t.bounds_max[i] for t in triangles) for i in range(3))
        node = cls(lo, hi)  # type: ignore[arg-type]
        if len(triangles) <= leaf_size:
            node.triangles = triangles
            return node
        axis = max(range(3), key=lambda i: hi[i] - lo[i])
        triangles.sort(key=lambda t: t.centroid[axis])
        mid = len(triangles) // 2
        node.left = cls.build(triangles[:mid], leaf_size)
        node.right = cls.build(triangles[mid:], leaf_size)
        return node

    @staticmethod
    def _box_hit(lo: Vec, hi: Vec, origin: Vec, direction: Vec, max_t: float) -> bool:
        near, far = 0.0, max_t
        for axis in range(3):
            d = direction[axis]
            if abs(d) < 1.0e-12:
                if origin[axis] < lo[axis] or origin[axis] > hi[axis]:
                    return False
                continue
            inv = 1.0 / d
            t0 = (lo[axis] - origin[axis]) * inv
            t1 = (hi[axis] - origin[axis]) * inv
            if t0 > t1:
                t0, t1 = t1, t0
            near = max(near, t0)
            far = min(far, t1)
            if far < near:
                return False
        return far > 1.0e-5

    def occluded(self, origin: Vec, direction: Vec, max_t: float) -> bool:
        if not self._box_hit(self.lo, self.hi, origin, direction, max_t):
            return False
        if self.triangles is not None:
            return any(t.intersect(origin, direction, max_t) is not None
                       for t in self.triangles)
        return ((self.left is not None and self.left.occluded(origin, direction, max_t)) or
                (self.right is not None and self.right.occluded(origin, direction, max_t)))

    def first_hit(self, origin: Vec, direction: Vec, max_t: float) -> Optional[tuple[float, Triangle]]:
        if not self._box_hit(self.lo, self.hi, origin, direction, max_t):
            return None
        best: Optional[tuple[float, Triangle]] = None
        if self.triangles is not None:
            for tri in self.triangles:
                t = tri.intersect(origin, direction, max_t if best is None else best[0])
                if t is not None and (best is None or t < best[0]):
                    best = (t, tri)
            return best
        for child in (self.left, self.right):
            if child is not None:
                hit = child.first_hit(origin, direction, max_t if best is None else best[0])
                if hit is not None and (best is None or hit[0] < best[0]):
                    best = hit
        return best


@dataclass
class ObjCorner:
    position_index: int
    texcoord_index: Optional[int]
    normal_index: Optional[int]


@dataclass
class ObjFace:
    corners: list[ObjCorner]
    smoothing: str
    normal: Vec
    material: str


def resolve_index(text: str, count: int) -> int:
    index = int(text)
    resolved = index - 1 if index > 0 else count + index
    if index == 0 or resolved < 0 or resolved >= count:
        raise ValueError(f"OBJ index {text!r} is out of range for {count} entries")
    return resolved


def read_obj(path: Path) -> tuple[list[Vec], list[Vec], list[ObjFace], list[str]]:
    positions: list[Vec] = []
    normals: list[Vec] = []
    faces: list[ObjFace] = []
    libraries: list[str] = []
    texcoord_count = 0
    smoothing = "off"
    material = ""
    for line_no, raw in enumerate(path.read_text(encoding="utf-8", errors="replace").splitlines(), 1):
        parts = raw.split('#', 1)[0].strip().split()
        if not parts or parts[0].startswith('#'):
            continue
        elif parts[0] == "v":
            if len(parts) < 4:
                raise ValueError(f"{path}:{line_no}: vertex needs x y z")
            positions.append((float(parts[1]), float(parts[2]), float(parts[3])))
        elif parts[0] == "vn":
            normals.append(normalize((float(parts[1]), float(parts[2]), float(parts[3]))))
        elif parts[0] == "vt":
            texcoord_count += 1
        elif parts[0] == "s":
            smoothing = parts[1] if len(parts) > 1 else "off"
        elif parts[0] == "usemtl":
            material = " ".join(shlex.split(raw.split('#', 1)[0])[1:])
        elif parts[0] == "mtllib":
            libraries.extend(shlex.split(raw.split('#', 1)[0])[1:])
        elif parts[0] == "f":
            corners = []
            for item in parts[1:]:
                fields = item.split('/')
                vi = resolve_index(fields[0], len(positions))
                ti = resolve_index(fields[1], texcoord_count) if len(fields) > 1 and fields[1] else None
                ni = resolve_index(fields[2], len(normals)) if len(fields) > 2 and fields[2] else None
                corners.append(ObjCorner(vi, ti, ni))
            if len(corners) < 3:
                raise ValueError(f"{path}:{line_no}: face has fewer than three vertices")
            p0 = positions[corners[0].position_index]
            face_normal = normalize(cross(sub(positions[corners[1].position_index], p0),
                                          sub(positions[corners[2].position_index], p0)))
            faces.append(ObjFace(corners, smoothing, face_normal, material))
        else:
            # Other records (UVs, materials, groups, etc.) are preserved by the writer.
            continue
    if not positions or not faces:
        raise ValueError(f"{path}: expected at least one vertex and one face")
    return positions, normals, faces, libraries


def read_materials(obj_path: Path, libraries: list[str], default_albedo: Vec) -> dict[str, tuple[Vec, Vec]]:
    materials: dict[str, tuple[Vec, Vec]] = {}
    for library in libraries:
        mtl_path = obj_path.parent / library
        if not mtl_path.is_file():
            print(f"warning: material library not found: {mtl_path}", file=sys.stderr)
            continue
        current: Optional[str] = None
        albedo = default_albedo
        emission: Vec = (0.0, 0.0, 0.0)
        for raw in mtl_path.read_text(encoding="utf-8", errors="replace").splitlines():
            fields = shlex.split(raw.split('#', 1)[0])
            if not fields:
                continue
            if fields[0] == "newmtl":
                if current is not None:
                    materials[current] = (albedo, emission)
                current = " ".join(fields[1:])
                albedo = default_albedo
                emission = (0.0, 0.0, 0.0)
            elif current is not None and fields[0] in ("Kd", "Ke") and len(fields) >= 4:
                color = (float(fields[1]), float(fields[2]), float(fields[3]))
                if fields[0] == "Kd":
                    albedo = color
                else:
                    emission = color
        if current is not None:
            materials[current] = (albedo, emission)
    return materials


def cosine_hemisphere(normal: Vec, rng: random.Random) -> Vec:
    r1, r2 = rng.random(), rng.random()
    radius = math.sqrt(r1)
    phi = 2.0 * math.pi * r2
    local = (radius * math.cos(phi), radius * math.sin(phi), math.sqrt(max(0.0, 1.0 - r1)))
    helper = (0.0, 0.0, 1.0) if abs(normal[2]) < 0.999 else (0.0, 1.0, 0.0)
    tangent = normalize(cross(helper, normal))
    bitangent = cross(normal, tangent)
    return normalize(add(add(mul(tangent, local[0]), mul(bitangent, local[1])),
                         mul(normal, local[2])))


@dataclass(frozen=True)
class PointLight:
    position: Vec
    color: Vec
    intensity: float
    radius: float


@dataclass
class BakeSettings:
    sky: Vec
    ground: Vec
    sun_dir: Vec
    sun_color: Vec
    ao_samples: int
    ao_distance: float
    ao_strength: float
    gi_samples: int
    gi_bounces: int
    gi_strength: float
    rr_depth: int
    point_lights: list[PointLight]
    seed: int
    ray_epsilon: float


class Baker:
    def __init__(self, root: BVHNode, settings: BakeSettings):
        self.root = root
        self.s = settings

    def sky_irradiance(self, normal: Vec) -> Vec:
        # A cheap hemispheric environment: upward-facing surfaces see sky,
        # downward-facing surfaces see ground. Colors are linear 0..1 values.
        t = clamp01(0.5 + 0.5 * normal[1])
        return add(mul(self.s.ground, 1.0 - t), mul(self.s.sky, t))

    def direct(self, point: Vec, normal: Vec, include_sky: bool = True) -> Vec:
        color = self.sky_irradiance(normal) if include_sky else (0.0, 0.0, 0.0)
        origin = add(point, mul(normal, self.s.ray_epsilon))
        ndotl = max(0.0, dot(normal, self.s.sun_dir))
        if ndotl > 0.0 and not self.root.occluded(origin, self.s.sun_dir, 1.0e30):
            color = add(color, mul(self.s.sun_color, ndotl))

        for light in self.s.point_lights:
            delta = sub(light.position, point)
            distance = length(delta)
            direction = normalize(delta)
            ndotl = max(0.0, dot(normal, direction))
            if distance >= light.radius or ndotl <= 0.0:
                continue
            if self.root.occluded(origin, direction, max(1.0e-5, distance - self.s.ray_epsilon)):
                continue
            radial = max(0.0, 1.0 - distance / light.radius)
            attenuation = light.intensity * radial * radial / max(0.25, distance * distance)
            color = add(color, mul(light.color, ndotl * attenuation))
        return color

    def trace_indirect(self, point: Vec, normal: Vec, rng: random.Random) -> Vec:
        """Trace one cosine-weighted path, reflecting from up to gi_bounces surfaces."""
        ray_origin = add(point, mul(normal, self.s.ray_epsilon))
        direction = cosine_hemisphere(normal, rng)
        throughput: Vec = (1.0, 1.0, 1.0)
        radiance: Vec = (0.0, 0.0, 0.0)

        for bounce in range(self.s.gi_bounces):
            hit = self.root.first_hit(ray_origin, direction, 1.0e30)
            if hit is None:
                # Sky at the primary vertex is already included by direct().
                # Sky seen after a surface is reflected indirect illumination.
                if bounce > 0:
                    radiance = add(radiance, hadamard(throughput, self.sky_irradiance(direction)))
                break

            distance, tri = hit
            hit_point = add(ray_origin, mul(direction, distance))
            hit_normal = tri.normal
            if dot(hit_normal, direction) > 0.0:
                hit_normal = mul(hit_normal, -1.0)

            radiance = add(radiance, hadamard(throughput, tri.emission))
            reflected_direct = hadamard(tri.albedo, self.direct(hit_point, hit_normal, include_sky=False))
            radiance = add(radiance, hadamard(throughput, reflected_direct))
            throughput = hadamard(throughput, tri.albedo)

            next_direction = cosine_hemisphere(hit_normal, rng)
            next_origin = add(hit_point, mul(hit_normal, self.s.ray_epsilon))
            if bounce + 1 >= self.s.gi_bounces:
                # Capture environment lighting after the final permitted surface bounce.
                if not self.root.occluded(next_origin, next_direction, 1.0e30):
                    radiance = add(radiance, hadamard(throughput, self.sky_irradiance(next_direction)))
                break

            if self.s.rr_depth > 0 and bounce + 1 >= self.s.rr_depth:
                survival = min(0.95, max(0.05, max(throughput)))
                if rng.random() >= survival:
                    break
                throughput = mul(throughput, 1.0 / survival)

            ray_origin, direction = next_origin, next_direction

        return radiance

    def bake_corner(self, point: Vec, normal: Vec, rng: random.Random) -> Vec:
        normal = normalize(normal)
        origin = add(point, mul(normal, self.s.ray_epsilon))

        ao = 0.0
        if self.s.ao_samples > 0 and self.s.ao_strength > 0.0:
            blocked = 0
            for _ in range(self.s.ao_samples):
                direction = cosine_hemisphere(normal, rng)
                if self.root.occluded(origin, direction, self.s.ao_distance):
                    blocked += 1
            ao = blocked / self.s.ao_samples

        result = self.direct(point, normal)
        if self.s.gi_samples > 0 and self.s.gi_strength > 0.0:
            indirect = (0.0, 0.0, 0.0)
            for _ in range(self.s.gi_samples):
                indirect = add(indirect, self.trace_indirect(point, normal, rng))
            result = add(result, mul(indirect, self.s.gi_strength / self.s.gi_samples))

        ao_factor = max(0.0, 1.0 - self.s.ao_strength * ao)
        result = mul(result, ao_factor)
        return tuple(clamp01(v) for v in result)  # type: ignore[return-value]


def corner_normals(positions: list[Vec], normals: list[Vec], faces: list[ObjFace]) -> list[list[Vec]]:
    smooth_sums: dict[tuple[int, str], Vec] = {}
    for face in faces:
        if face.smoothing.lower() not in ("off", "0"):
            for corner in face.corners:
                key = (corner.position_index, face.smoothing)
                smooth_sums[key] = add(smooth_sums.get(key, (0.0, 0.0, 0.0)), face.normal)
    result = []
    for face in faces:
        row = []
        for corner in face.corners:
            if corner.normal_index is not None and 0 <= corner.normal_index < len(normals):
                n = normals[corner.normal_index]
            elif face.smoothing.lower() in ("off", "0"):
                n = face.normal
            else:
                n = normalize(smooth_sums.get((corner.position_index, face.smoothing), face.normal))
            row.append(n)
        result.append(row)
    return result


def write_baked_obj(path: Path, out_path: Path, positions: list[Vec],
                    faces: list[ObjFace], colors: list[list[Vec]]) -> None:
    # Rebuild in source order, duplicating each face corner so color and normal
    # boundaries are retained without changing UV or normal indices.
    input_lines = path.read_text(encoding="utf-8", errors="replace").splitlines()
    out: list[str] = []
    face_cursor = 0
    generated_vertex = 0
    for raw in input_lines:
        parts = raw.split('#', 1)[0].strip().split()
        if parts and parts[0] == "v":
            continue
        if parts and parts[0] == "mtllib":
            libraries = shlex.split(raw.split('#', 1)[0])[1:]
            relative_libraries = [os.path.relpath((path.parent / name).resolve(), out_path.parent.resolve())
                                  for name in libraries]
            out.append("mtllib " + " ".join(shlex.quote(name) for name in relative_libraries))
        elif parts and parts[0] == "f":
            face = faces[face_cursor]
            corner_colors = colors[face_cursor]
            face_cursor += 1
            refs = []
            for corner, color in zip(face.corners, corner_colors):
                generated_vertex += 1
                p = positions[corner.position_index]
                out.append("v {:.7g} {:.7g} {:.7g} {:.7g} {:.7g} {:.7g}".format(*p, *color))
                ti = corner.texcoord_index
                ni = corner.normal_index
                if ti is not None and ni is not None:
                    ref = f"{generated_vertex}/{ti + 1}/{ni + 1}"
                elif ti is not None:
                    ref = f"{generated_vertex}/{ti + 1}"
                elif ni is not None:
                    ref = f"{generated_vertex}//{ni + 1}"
                else:
                    ref = str(generated_vertex)
                refs.append(ref)
            out.append("f " + " ".join(refs))
        elif parts and parts[0] == "vn":
            out.append(raw)
        elif parts and parts[0] == "vt":
            out.append(raw)
        else:
            out.append(raw)
    out_path.write_text("\n".join(out) + "\n", encoding="utf-8")


def parse_args(argv: Optional[Iterable[str]] = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("input", type=Path, help="source Wavefront OBJ")
    parser.add_argument("output", type=Path, help="output OBJ with baked RGB vertex colors")
    parser.add_argument("--sky", nargs=3, type=float, default=(0.28, 0.34, 0.45), metavar=("R", "G", "B"),
                        help="hemispheric sky radiance, linear 0..1 (default: %(default)s)")
    parser.add_argument("--ground", nargs=3, type=float, default=(0.045, 0.035, 0.025), metavar=("R", "G", "B"),
                        help="lower-hemisphere/ground fill (default: %(default)s)")
    parser.add_argument("--sun-dir", nargs=3, type=float, default=(0.35, 0.8, 0.25), metavar=("X", "Y", "Z"),
                        help="direction from surfaces toward the sun")
    parser.add_argument("--sun-color", nargs=3, type=float, default=(0.8, 0.72, 0.58), metavar=("R", "G", "B"),
                        help="direct sun radiance, linear 0..1")
    parser.add_argument("--ao-samples", type=int, default=32, help="cosine hemisphere AO rays per corner")
    parser.add_argument("--ao-distance", type=float, default=2.0, help="AO ray limit in OBJ units")
    parser.add_argument("--ao-strength", type=float, default=0.75, help="AO darkening from 0 to 1")
    parser.add_argument("--gi-samples", type=int, default=16,
                        help="Monte Carlo GI paths per corner; 0 disables GI")
    parser.add_argument("--gi-bounces", type=int, default=8,
                        help="maximum diffuse surface interactions per GI path")
    parser.add_argument("--gi-strength", type=float, default=0.65,
                        help="scale for indirect illumination")
    parser.add_argument("--rr-depth", type=int, default=3,
                        help="start Russian roulette after this many bounces; 0 disables it")
    parser.add_argument("--default-albedo", "--bounce-albedo", dest="default_albedo",
                        nargs=3, type=float, default=(0.72, 0.68, 0.62), metavar=("R", "G", "B"),
                        help="fallback diffuse reflectance when an MTL Kd is missing")
    parser.add_argument("--point-light", nargs=8, action="append", type=float,
                        metavar=("X", "Y", "Z", "R", "G", "B", "INTENSITY", "RADIUS"),
                        help="baked point light; repeat for multiple lights (OBJ units)")
    parser.add_argument("--epsilon", type=float, default=1.0e-4, help="ray origin offset in OBJ units")
    parser.add_argument("--monochrome", action="store_true",
                        help="write grayscale baked RGB, matching Quantum mode 9")
    parser.add_argument("--seed", type=int, default=13, help="deterministic sample seed")
    args = parser.parse_args(argv)
    for name in ("sky", "ground", "sun_color", "default_albedo"):
        if any(v < 0.0 for v in getattr(args, name)):
            parser.error(f"--{name.replace('_', '-')} values must be non-negative")
    for name in ("ao_strength", "gi_strength"):
        if getattr(args, name) < 0.0:
            parser.error(f"--{name.replace('_', '-')} must be non-negative")
    if (args.ao_samples < 0 or args.gi_samples < 0 or args.gi_bounces < 0 or
            args.rr_depth < 0 or args.ao_distance <= 0 or args.epsilon <= 0):
        parser.error("sample counts, bounce count and roulette depth must be non-negative; distances must be positive")
    for values in args.point_light or []:
        if any(v < 0.0 for v in values[3:6]) or values[6] < 0.0 or values[7] <= 0.0:
            parser.error("point-light colors/intensity must be non-negative and radius must be positive")
    args.sun_dir = normalize(args.sun_dir)
    args.point_lights = [PointLight(tuple(v[:3]), tuple(v[3:6]), v[6], v[7])
                         for v in (args.point_light or [])]
    return args


def main(argv: Optional[Iterable[str]] = None) -> int:
    args = parse_args(argv)
    try:
        positions, normals, faces, libraries = read_obj(args.input)
        materials = read_materials(args.input, libraries, tuple(args.default_albedo))
        triangles: list[Triangle] = []
        for face in faces:
            albedo, emission = materials.get(
                face.material, (tuple(args.default_albedo), (0.0, 0.0, 0.0)))
            origin = positions[face.corners[0].position_index]
            for i in range(1, len(face.corners) - 1):
                triangles.append(Triangle.make(
                    origin,
                    positions[face.corners[i].position_index],
                    positions[face.corners[i + 1].position_index],
                    albedo, emission))
        bvh = BVHNode.build(triangles)
        settings = BakeSettings(args.sky, args.ground, args.sun_dir, args.sun_color,
                                args.ao_samples, args.ao_distance, args.ao_strength,
                                args.gi_samples, args.gi_bounces, args.gi_strength,
                                args.rr_depth, args.point_lights, args.seed, args.epsilon)
        baker = Baker(bvh, settings)
        normals_by_face = corner_normals(positions, normals, faces)
        all_colors: list[list[Vec]] = []
        rng = random.Random(args.seed)
        total = sum(len(face.corners) for face in faces)
        done = 0
        for face, face_normals in zip(faces, normals_by_face):
            row = []
            for corner, normal in zip(face.corners, face_normals):
                color = baker.bake_corner(positions[corner.position_index], normal, rng)
                if args.monochrome:
                    luminance = 0.2126 * color[0] + 0.7152 * color[1] + 0.0722 * color[2]
                    color = (luminance, luminance, luminance)
                row.append(color)
                done += 1
                if done % 500 == 0 or done == total:
                    print(f"Baking vertex corners: {done}/{total}", file=sys.stderr)
            all_colors.append(row)
        write_baked_obj(args.input, args.output, positions, faces, all_colors)
        print(f"Wrote RGB vertex illumination to {args.output}", file=sys.stderr)
    except (OSError, ValueError, IndexError) as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 2
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
