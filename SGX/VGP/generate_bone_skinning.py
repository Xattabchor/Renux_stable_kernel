#!/usr/bin/env python3
"""Generate and compile the largest VGP affine bone palette the SDK accepts.

The shader uses dynamic addressing and four sequential influences, reusing a
small TEMP set. A 3x4 affine matrix is uploaded as three vec4 parameter rows,
split across local and environment parameter banks when needed.
"""
import argparse
import os
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
DEFAULT_COMPILER = ROOT / "SGX/SDK_BIN/Graphics_SDK_setuplinux_4_03_00_02/GFX_Linux_SDK/OGLES/SDKPackage/Utilities/VGPCompiler/Linux/VGPCompiler"


def make_source(bones: int) -> str:
    rows = bones * 3
    local_rows = min(rows, 16)
    env_rows = rows - local_rows
    param_ranges = []
    if local_rows:
        param_ranges.append("program.local[0..%d]" % (local_rows - 1))
    if env_rows:
        param_ranges.append("program.env[0..%d]" % (env_rows - 1))
    lines = [
        "!!IMGvp1.0",
        "# Full VGP linear-blend skinning probe; bone rows are object-space 3x4 affine matrices.",
        "# Upload the first %d rows to local parameters and the remaining %d to environment parameters." % (local_rows, env_rows),
        "# vertex.attrib[1] = four integer-valued bone indices; attrib[2] = four weights.",
        "# Output color visualizes the normalized skinned normal for validation.",
        "ATTRIB iPos = vertex.position;",
        "ATTRIB iNormal = vertex.normal;",
        "ATTRIB iBones = vertex.attrib[1];",
        "ATTRIB iWeights = vertex.attrib[2];",
        "ATTRIB iUV = vertex.texcoord[0];",
        "PARAM mvp[4] = { state.matrix.mvp };",
        "PARAM boneRows[%d] = { %s };" % (rows, ", ".join(param_ranges)),
        "TEMP pAcc, nAcc, pTmp, nTmp;",
        "ADDRESS A0;",
        "MOV pAcc, 0.0;",
        "MOV nAcc, 0.0;",
    ]
    for influence, component in enumerate("xyzw"):
        lines.extend([
            "# influence %d: reuse the same address and transform temporaries" % influence,
            "MUL pTmp.x, iBones.%s, 3.0;" % component,
            "ARL A0.x, pTmp.x;",
            "DP4 pTmp.x, boneRows[A0.x + 0], iPos;",
            "DP4 pTmp.y, boneRows[A0.x + 1], iPos;",
            "DP4 pTmp.z, boneRows[A0.x + 2], iPos;",
            "MAD pAcc.xyz, pTmp, iWeights.%s, pAcc;" % component,
            "MAD pAcc.w, iWeights.%s, 1.0, pAcc.w;" % component,
            "DP3 nTmp.x, boneRows[A0.x + 0], iNormal;",
            "DP3 nTmp.y, boneRows[A0.x + 1], iNormal;",
            "DP3 nTmp.z, boneRows[A0.x + 2], iNormal;",
            "MAD nAcc.xyz, nTmp, iWeights.%s, nAcc;" % component,
        ])
    lines.extend([
        "DP4 result.position.x, mvp[0], pAcc;",
        "DP4 result.position.y, mvp[1], pAcc;",
        "DP4 result.position.z, mvp[2], pAcc;",
        "DP4 result.position.w, mvp[3], pAcc;",
        "DP3 pTmp.x, nAcc, nAcc;",
        "MAX pTmp.x, pTmp.x, 0.0001;",
        "RSQ pTmp.x, pTmp.x;",
        "MUL nAcc.xyz, nAcc, pTmp.x;",
        "MAD result.color.xyz, nAcc, 0.5, 0.5;",
        "MOV result.color.w, 1.0;",
        "MOV result.texcoord[0], iUV;",
        "END",
        "",
    ])
    return "\n".join(lines)


def compile_one(compiler: Path, source: Path, output_base: Path) -> tuple[bool, str]:
    log_file = output_base.with_suffix(".log")
    proc = subprocess.run(
        [str(compiler), "-i" + str(source), "-o" + str(output_base), "-target=generic"],
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
    )
    log = proc.stdout or ""
    log_file.write_text(log)
    success = proc.returncode == 0 and "Compiled OK." in log and output_base.with_suffix(".bin").is_file()
    return success, log


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--compiler", type=Path, default=DEFAULT_COMPILER)
    parser.add_argument("--out-dir", type=Path, default=ROOT / "SGX/VGP/build")
    parser.add_argument("--max-try", type=int, default=64)
    parser.add_argument("--min-bones", type=int, default=1)
    args = parser.parse_args()

    compiler = args.compiler.resolve()
    out_dir = args.out_dir.resolve()
    out_dir.mkdir(parents=True, exist_ok=True)
    report = []
    last_success = 0
    last_base = None
    failure_reason = "No candidate was attempted."

    if not compiler.is_file():
        print("VGPCompiler not found: %s" % compiler, file=sys.stderr)
        return 2

    with tempfile.TemporaryDirectory(prefix="vgp-bones-") as tmp:
        temp_dir = Path(tmp)
        for bones in range(args.min_bones, args.max_try + 1):
            source = temp_dir / ("BoneSkinning_%02d.vp" % bones)
            output_base = temp_dir / ("BoneSkinning_%02d" % bones)
            source.write_text(make_source(bones))
            success, log = compile_one(compiler, source, output_base)
            status = "PASS" if success else "FAIL"
            report.append("=== %d bones: %s ===\n%s" % (bones, status, log))
            if not success:
                failure_reason = "First non-compiling palette size: %d bones." % bones
                break
            last_success = bones
            last_base = output_base

        (out_dir / "BoneSkinning_search.log").write_text("\n".join(report) + "\n")
        if last_success == 0 or last_base is None:
            print(failure_reason, file=sys.stderr)
            return 1

        final_source = out_dir / "BoneSkinningMax.vp"
        final_source.write_text(make_source(last_success))
        shutil.copyfile(last_base.with_suffix(".bin"), out_dir / "BoneSkinningMax.bin")
        shutil.copyfile(last_base.with_suffix(".h"), out_dir / "BoneSkinningMax.h")

    rows = last_success * 3
    local_rows = min(rows, 16)
    env_rows = rows - local_rows
    summary = "Maximum contiguous palette compiled: %d bones (3 vec4 rows per bone, %d parameter rows: %d local + %d environment)." % (
        last_success, rows, local_rows, env_rows
    )
    (out_dir / "BoneSkinningMax.txt").write_text(
        summary + "\n" + failure_reason + "\nTEMP declaration: 4 vec4 registers.\n"
    )
    print(summary)
    print(failure_reason)
    return 0


if __name__ == "__main__":
    sys.exit(main())
