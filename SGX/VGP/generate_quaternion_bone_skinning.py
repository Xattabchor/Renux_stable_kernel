#!/usr/bin/env python3
"""Probe quaternion-plus-translation bone palettes using the SDK VGPCompiler."""
import argparse
import shutil
import sys
import tempfile
from pathlib import Path

from generate_bone_skinning import DEFAULT_COMPILER, compile_one


def make_source(bones: int) -> str:
    rows = bones * 2
    local_rows = min(rows, 16)
    env_rows = rows - local_rows
    param_ranges = []
    if local_rows:
        param_ranges.append("program.local[0..%d]" % (local_rows - 1))
    if env_rows:
        param_ranges.append("program.env[0..%d]" % (env_rows - 1))
    lines = [
        "!!IMGvp1.0",
        "# Quaternion skinning: two vec4 parameter rows per rigid bone.",
        "# Row 2*b is unit quaternion (x,y,z,w); row 2*b+1 is translation (x,y,z,unused).",
        "# vertex.attrib[1] = four integer-valued bone indices; attrib[2] = four weights.",
        "# Bone indices are in [0,%d]; input quaternions must be normalized." % (bones - 1),
        "ATTRIB iPos = vertex.position;",
        "ATTRIB iNormal = vertex.normal;",
        "ATTRIB iBones = vertex.attrib[1];",
        "ATTRIB iWeights = vertex.attrib[2];",
        "ATTRIB iUV = vertex.texcoord[0];",
        "PARAM mvp[4] = { state.matrix.mvp };",
        "PARAM boneRows[%d] = { %s };" % (rows, ", ".join(param_ranges)),
        "TEMP pAcc, nAcc, outTmp, crossTmp, scalarTmp;",
        "ADDRESS A0;",
        "MOV pAcc, 0.0;",
        "MOV nAcc, 0.0;",
    ]
    for influence, component in enumerate("xyzw"):
        q = "boneRows[A0.x + 0]"
        t = "boneRows[A0.x + 1]"
        lines.extend([
            "# influence %d: reconstruct rotation from a unit quaternion" % influence,
            "MUL scalarTmp.x, iBones.%s, 2.0;" % component,
            "ARL A0.x, scalarTmp.x;",
            # c = q.xyz x position.xyz
            "MUL crossTmp.x, %s.y, iPos.z;" % q,
            "MUL scalarTmp.x, %s.z, iPos.y;" % q,
            "SUB crossTmp.x, crossTmp.x, scalarTmp.x;",
            "MUL crossTmp.y, %s.z, iPos.x;" % q,
            "MUL scalarTmp.x, %s.x, iPos.z;" % q,
            "SUB crossTmp.y, crossTmp.y, scalarTmp.x;",
            "MUL crossTmp.z, %s.x, iPos.y;" % q,
            "MUL scalarTmp.x, %s.y, iPos.x;" % q,
            "SUB crossTmp.z, crossTmp.z, scalarTmp.x;",
            # out = position + 2 * (q.w*c + q.xyz x c)
            "MUL outTmp.x, %s.y, crossTmp.z;" % q,
            "MUL scalarTmp.x, %s.z, crossTmp.y;" % q,
            "SUB outTmp.x, outTmp.x, scalarTmp.x;",
            "MAD outTmp.x, %s.w, crossTmp.x, outTmp.x;" % q,
            "MUL outTmp.x, outTmp.x, 2.0;",
            "ADD outTmp.x, outTmp.x, iPos.x;",
            "MUL outTmp.y, %s.z, crossTmp.x;" % q,
            "MUL scalarTmp.x, %s.x, crossTmp.z;" % q,
            "SUB outTmp.y, outTmp.y, scalarTmp.x;",
            "MAD outTmp.y, %s.w, crossTmp.y, outTmp.y;" % q,
            "MUL outTmp.y, outTmp.y, 2.0;",
            "ADD outTmp.y, outTmp.y, iPos.y;",
            "MUL outTmp.z, %s.x, crossTmp.y;" % q,
            "MUL scalarTmp.x, %s.y, crossTmp.x;" % q,
            "SUB outTmp.z, outTmp.z, scalarTmp.x;",
            "MAD outTmp.z, %s.w, crossTmp.z, outTmp.z;" % q,
            "MUL outTmp.z, outTmp.z, 2.0;",
            "ADD outTmp.z, outTmp.z, iPos.z;",
            "ADD outTmp.xyz, outTmp, %s;" % t,
            "MOV outTmp.w, 1.0;",
            "MAD pAcc.xyz, outTmp, iWeights.%s, pAcc;" % component,
            "MAD pAcc.w, iWeights.%s, 1.0, pAcc.w;" % component,
            # c = q.xyz x normal.xyz
            "MUL crossTmp.x, %s.y, iNormal.z;" % q,
            "MUL scalarTmp.x, %s.z, iNormal.y;" % q,
            "SUB crossTmp.x, crossTmp.x, scalarTmp.x;",
            "MUL crossTmp.y, %s.z, iNormal.x;" % q,
            "MUL scalarTmp.x, %s.x, iNormal.z;" % q,
            "SUB crossTmp.y, crossTmp.y, scalarTmp.x;",
            "MUL crossTmp.z, %s.x, iNormal.y;" % q,
            "MUL scalarTmp.x, %s.y, iNormal.x;" % q,
            "SUB crossTmp.z, crossTmp.z, scalarTmp.x;",
            # out = normal + 2 * (q.w*c + q.xyz x c)
            "MUL outTmp.x, %s.y, crossTmp.z;" % q,
            "MUL scalarTmp.x, %s.z, crossTmp.y;" % q,
            "SUB outTmp.x, outTmp.x, scalarTmp.x;",
            "MAD outTmp.x, %s.w, crossTmp.x, outTmp.x;" % q,
            "MUL outTmp.x, outTmp.x, 2.0;",
            "ADD outTmp.x, outTmp.x, iNormal.x;",
            "MUL outTmp.y, %s.z, crossTmp.x;" % q,
            "MUL scalarTmp.x, %s.x, crossTmp.z;" % q,
            "SUB outTmp.y, outTmp.y, scalarTmp.x;",
            "MAD outTmp.y, %s.w, crossTmp.y, outTmp.y;" % q,
            "MUL outTmp.y, outTmp.y, 2.0;",
            "ADD outTmp.y, outTmp.y, iNormal.y;",
            "MUL outTmp.z, %s.x, crossTmp.y;" % q,
            "MUL scalarTmp.x, %s.y, crossTmp.x;" % q,
            "SUB outTmp.z, outTmp.z, scalarTmp.x;",
            "MAD outTmp.z, %s.w, crossTmp.z, outTmp.z;" % q,
            "MUL outTmp.z, outTmp.z, 2.0;",
            "ADD outTmp.z, outTmp.z, iNormal.z;",
            "MAD nAcc.xyz, outTmp, iWeights.%s, nAcc;" % component,
        ])
    lines.extend([
        "DP4 result.position.x, mvp[0], pAcc;",
        "DP4 result.position.y, mvp[1], pAcc;",
        "DP4 result.position.z, mvp[2], pAcc;",
        "DP4 result.position.w, mvp[3], pAcc;",
        "DP3 scalarTmp.x, nAcc, nAcc;",
        "MAX scalarTmp.x, scalarTmp.x, 0.0001;",
        "RSQ scalarTmp.x, scalarTmp.x;",
        "MUL nAcc.xyz, nAcc, scalarTmp.x;",
        "MAD result.color.xyz, nAcc, 0.5, 0.5;",
        "MOV result.color.w, 1.0;",
        "MOV result.texcoord[0], iUV;",
        "END",
        "",
    ])
    return "\n".join(lines)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--compiler", type=Path, default=DEFAULT_COMPILER)
    parser.add_argument("--out-dir", type=Path, default=Path(__file__).resolve().parent / "build")
    parser.add_argument("--max-try", type=int, default=64)
    args = parser.parse_args()
    compiler = args.compiler.resolve()
    out_dir = args.out_dir.resolve()
    out_dir.mkdir(parents=True, exist_ok=True)
    if not compiler.is_file():
        print("VGPCompiler not found: %s" % compiler, file=sys.stderr)
        return 2

    report = []
    best = 0
    best_base = None
    reason = "No candidate was attempted."
    with tempfile.TemporaryDirectory(prefix="vgp-quaternion-bones-") as tmp:
        temp_dir = Path(tmp)
        for bones in range(1, args.max_try + 1):
            src = temp_dir / ("BoneSkinningQuaternion_%02d.vp" % bones)
            base = temp_dir / ("BoneSkinningQuaternion_%02d" % bones)
            src.write_text(make_source(bones))
            success, log = compile_one(compiler, src, base)
            report.append("=== %d bones: %s ===\n%s" % (bones, "PASS" if success else "FAIL", log))
            if not success:
                reason = "First non-compiling palette size: %d bones." % bones
                break
            best, best_base = bones, base
        (out_dir / "BoneSkinningQuaternion_search.log").write_text("\n".join(report) + "\n")
        if best_base is None:
            print(reason, file=sys.stderr)
            return 1
        (out_dir / "BoneSkinningQuaternionMax.vp").write_text(make_source(best))
        shutil.copyfile(best_base.with_suffix(".bin"), out_dir / "BoneSkinningQuaternionMax.bin")
        shutil.copyfile(best_base.with_suffix(".h"), out_dir / "BoneSkinningQuaternionMax.h")

    local_rows = min(best * 2, 16)
    env_rows = best * 2 - local_rows
    summary = "Maximum quaternion palette compiled: %d bones (%d vec4 rows: %d local + %d environment)." % (
        best, best * 2, local_rows, env_rows
    )
    (out_dir / "BoneSkinningQuaternionMax.txt").write_text(summary + "\n" + reason + "\nTEMP declaration: 5 vec4 registers.\n")
    print(summary)
    print(reason)
    return 0


if __name__ == "__main__":
    sys.exit(main())
