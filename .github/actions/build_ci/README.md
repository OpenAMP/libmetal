# Libmetal Build Check Docker Action

This action test builds for a specified target.

## Inputs

### `Target`

**Required** the build target. Default `"linux"`.

The supported targets are:

* linux
* generic arm
* freertos
* zephyr


## Example Usage

```
uses: ./.github/actions/build_ci
with:
  target: freertos
```
