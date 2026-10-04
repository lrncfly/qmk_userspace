# QMK Userspace (`qmk_userspace`)

Personal QMK userspace configuration repository for custom split mechanical keyboard firmware, keymaps, and community module integrations.

Official Bastard Keyboards keymaps/docs here: [https://docs.bastardkb.com/fw/compile-firmware.html](https://docs.bastardkb.com/fw/compile-firmware.html).

---

## 🛠 Related Repositories & Submodule Structure

This workspace relies on an ecosystem of personal forks and submodules to build firmware while maintaining isolation from upstream source code:

* **Main QMK Firmware Engine (`bastardkb-qmk`)**:
  * `origin`: [`lrncfly/bastardkb-qmk`](https://github.com/lrncfly/bastardkb-qmk)
  * `upstream`: [`Bastardkb/bastardkb-qmk`](https://github.com/Bastardkb/bastardkb-qmk)

* **Userspace Repository (`qmk_userspace`)**:
  * `origin`: [`lrncfly/qmk_userspace`](https://github.com/lrncfly/qmk_userspace)
  * `upstream`: [`Bastardkb/qmk_userspace`](https://github.com/Bastardkb/qmk_userspace)

* **Personal Custom Modules (`modules/lrncfly`)**:
  * `origin`: [`lrncfly/qmk_modules`](https://github.com/lrncfly/qmk_modules)

* **Bastardkb Modules Submodule (`modules/bastardkb`)**:
  * `origin`: [`lrncfly/bastardkb-qmk_modules`](https://github.com/lrncfly/bastardkb-qmk_modules)
  * `upstream`: [`Bastardkb/qmk_modules`](https://github.com/Bastardkb/qmk_modules)

---

## 🚀 Cloning & Initial Setup

When cloning this workspace on a new machine or inside a Dev Container/WSL environment:

```bash
# Clone with recursive submodules
git clone --recursive https://github.com/lrncfly/qmk_userspace.git
cd qmk_userspace

# If cloned without --recursive, initialize submodules manually
git submodule update --init --recursive
```

### Submodule Remotes Verification

To verify that all submodule paths and remotes are correctly set up inside the workspace:

```bash
# Verify personal modules repository
cd modules/lrncfly
git remote -v
cd ../..

# Verify Bastardkb modules fork and upstream remotes
cd modules/bastardkb
git remote -v
cd ../..
```

*Expected output for `modules/bastardkb`:*
```text
origin    https://github.com/lrncfly/bastardkb-qmk_modules.git (fetch/push)
upstream  https://github.com/Bastardkb/qmk_modules.git (fetch/push)
```

---

## 🔄 Daily Workflow

### 1. Making Changes to Submodules

When editing files inside either submodule directory (`modules/lrncfly` or `modules/bastardkb`):

```bash
# 1. Navigate into the relevant submodule directory
cd modules/bastardkb   # or cd modules/lrncfly

# 2. Commit and push changes to YOUR repository/fork
git add .
git commit -m "feat: add custom module updates"
git push origin <branch-name>

# 3. Step back to root workspace and lock the commit pointer in qmk_userspace
cd ../..
git add modules/
git commit -m "chore: update submodule commit pointers"
git push origin <branch-name>
```

> **Note:** Always remember to commit inside the submodule **first**, then commit the updated commit pointer in the parent `qmk_userspace` repository.

---

### 2. Syncing Upstream Changes

#### Submodules (`modules/bastardkb`)
```bash
cd modules/bastardkb
git fetch upstream
git rebase upstream/main
git push origin main --force-with-lease

cd ../..
git add modules/bastardkb
git commit -m "chore: sync bastardkb submodule with upstream"
git push origin main
```

#### Core Firmware (`bastardkb-qmk`)
```bash
cd ../bastardkb-qmk
git checkout bkb-develop
git fetch upstream
git rebase upstream/Bastardkb-bkb-develop
git push origin bkb-develop --force-with-lease
```

---

## ⚙️ CI/CD & Automated Builds

This repository utilizes GitHub Actions to automatically compile firmware binaries upon push. 

* `.gitmodules` explicitly tracks both `lrncfly/qmk_modules` and `lrncfly/bastardkb-qmk_modules`.
* CI runners automatically resolve and check out the exact submodule commit hashes recorded by `qmk_userspace` and compile against the custom `bastardkb-qmk` base.

---

## 🔨 Local Compilation

To run a test compilation locally via QMK CLI:

```bash
# Clean previous build artifacts
qmk clean

# Compile keymap for Dilemma 3x5_3
qmk compile -c -kb bastardkb/dilemma/3x5_3_procyon -km lrncfly
```
