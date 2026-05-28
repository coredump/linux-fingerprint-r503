# Arch Linux package — r503d

Builds and installs `r503d` as a `pacman`-tracked package. Conflicts with and
replaces `fprintd` at the package-metadata level.

## Build and install

```bash
cd packaging/arch
makepkg -si
```

`makepkg -si` compiles the daemon (`cargo build --release`) and hands the
`.pkg.tar.zst` to `pacman` for installation. Tests run as part of `check()`;
skip them with `makepkg -si --nocheck`.

## Remove

```bash
sudo pacman -Rns r503d
```

`-n` removes the package, `-s` removes unneeded deps, `-n` is a no-op here
(no backup files). Runtime state (`/var/lib/r503d/`, `/etc/r503d/`) is
deliberately preserved — remove manually if you want a clean slate.

## Notes

This PKGBUILD builds directly from the local checkout via `$startdir/../..`.
It is **not AUR-submittable** as-is. To publish on AUR, replace `source=()` with
`source=("git+https://github.com/matpb/linux-fingerprint-r503.git#tag=v$pkgver")`.

`dist/install.sh` remains functional for non-Arch hosts and is not modified.
