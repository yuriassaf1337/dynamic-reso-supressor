# dynamic-reso-supressor

this is a fork from [original](https://github.com/Thiagohgl/DyERS-dynamic-resonance-suppressor)
read the original README.md in og_README.md.

<img width="1200" height="406" alt="UI" src="https://github.com/user-attachments/assets/5ac336a7-a177-410f-bc4d-219507a460a0" />

## building

this project cross-compiles a Windows VST3 from Linux using MinGW-w64 and JUCE 8.

**prerequisites:** `cmake`, `mingw-w64`, `wine` (to run build helpers during compilation).

clone JUCE into the repo root before building:

```bash
git clone --depth=1 https://github.com/juce-framework/JUCE.git
```

### d-r-s (scripts)

`drs` is the build dispatcher at the repo root. configuration defaults live in `scripts/config.sh` — edit that file or override any variable via the environment.

```bash
./drs <command>
```

| command | what it does |
|---|---|
| `build` | configure cmake (first run only) then build the vst3 |
| `install` | copy the built `.vst3` bundle to `VST3_INSTALL_DIR` |
| `rebuild` | clean + build + install in one shot |
| `clean` | wipe the build directory |
| `help` | show usage and the current resolved config values |

**typical workflow:**

```bash
./drs rebuild
```

**override the install path without editing the file:**

```bash
VST3_INSTALL_DIR="/path/to/your/vst3/folder" ./drs rebuild
```

**native linux vst3** (no cross-compilation):

```bash
TOOLCHAIN_FILE="" VST3_INSTALL_DIR="$HOME/.vst3" ./drs rebuild
```

**just build, no install:**

```bash
./drs build
```
