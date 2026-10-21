#!/usr/bin/env python3
"""Compare VGPCompiler resource ceilings across generic, ARM VP and VGP Lite."""
from pathlib import Path
import re
import subprocess
import sys
import tempfile

compiler = Path(sys.argv[1]).resolve()
out_dir = Path('vgp-target-probe/out')
out_dir.mkdir(parents=True, exist_ok=True)
targets = {
    'generic': ('-target=generic',),
    'armvp': ('-target=armvp',),
    'vgplite': ('-vgplite', '-target=generic'),
}


def run(name, body, flags):
    src = f'!!IMGvp1.0\n{body}\nEND\n'
    with tempfile.TemporaryDirectory(prefix='vgp-probe-') as tmp:
        source = Path(tmp) / f'{name}.vp'
        output = out_dir / name
        source.write_text(src)
        cmd = [str(compiler), f'-i{source}', f'-o{output}', *flags]
        proc = subprocess.run(cmd, text=True, capture_output=True)
        text = (proc.stdout + '\n' + proc.stderr).strip()
        (out_dir / f'{name}.log').write_text(
            f'probe={name}\nexit={proc.returncode}\ncommand={" ".join(cmd)}\n{text}\n')
        for suffix in ('.h', '.bin'):
            Path(str(output) + suffix).unlink(missing_ok=True)
        print(f'===== {name} (exit {proc.returncode}) =====\n{text}\n')
        return text


for target, flags in targets.items():
    for bank in ('local', 'env'):
        body = (f'PARAM p[256] = {{ program.{bank}[0..255] }};\n'
                'TEMP sink;\nMOV sink, p[0];\nMOV result.position, vertex.position;')
        text = run(f'{target}_{bank}_overflow', body, flags)
        match = re.search(r'max is (\d+)', text)
        if match:
            count = int(match.group(1))
            for requested in (count, count + 1):
                last = requested - 1
                body = (f'PARAM p[{requested}] = {{ program.{bank}[0..{last}] }};\n'
                        'TEMP sink;\nMOV sink, p[0];\nMOV result.position, vertex.position;')
                run(f'{target}_{bank}_{requested}', body, flags)

    body = 'TEMP sink;\nMOV sink, vertex.position;\n'
    body += 'ADD sink, sink, vertex.position;\n' * 510
    body += 'MOV result.position, sink;'
    run(f'{target}_instructions_512', body, ('-do', *flags))

    ntemps = 64
    names = ', '.join(f't{i}' for i in range(ntemps))
    body = f'TEMP {names};\n'
    body += ''.join(f'MOV t{i}, vertex.position;\n' for i in range(ntemps))
    body += f'MOV result.position, t{ntemps-1};'
    run(f'{target}_temps_{ntemps}', body, ('-do', *flags))

print(f'Probe logs saved in {out_dir}')
