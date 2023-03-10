{
  description = "ll_alloc";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-parts = {
      url = "github:hercules-ci/flake-parts";
      inputs.nixpkgs-lib.follows = "nixpkgs";
    };
    pre-commit-hooks-nix = {
      url = "github:cachix/pre-commit-hooks.nix";
      inputs.nixpkgs.follows = "nixpkgs";
      inputs.nixpkgs-stable.follows = "nixpkgs";
    };
    nix-filter.url = "github:numtide/nix-filter";
    t4-nix = {
      url = "github:t4ccer/t4.nix";
      inputs.nixpkgs.follows = "nixpkgs";
      inputs.flake-parts.follows = "flake-parts";
      inputs.pre-commit-hooks-nix.follows = "pre-commit-hooks-nix";
    };
  };

  outputs = inputs @ {flake-parts, ...}: let
    nix-filter = import inputs.nix-filter;
  in
    flake-parts.lib.mkFlake {inherit inputs;} {
      imports = [
        inputs.pre-commit-hooks-nix.flakeModule
        inputs.t4-nix.stdenvMatrix
      ];

      # Hack to make following work with IFD:
      # nix flake show --impure --allow-import-from-derivation
      systems =
        if builtins.hasAttr "currentSystem" builtins
        then [builtins.currentSystem]
        else inputs.nixpkgs.lib.systems.flakeExposed;

      perSystem = {
        config,
        self',
        inputs',
        pkgs,
        system,
        ...
      }: let
        inherit (pkgs) lib;
        version = "1.0.0";
        stdenvs = ["stdenv" "clangStdenv"];
      in {
        pre-commit.settings = {
          hooks = {
            alejandra.enable = true;
            clang-format.enable = true;
          };
        };

        devShells.default = pkgs.mkShell {
          shellHook = config.pre-commit.installationScript;
          nativeBuildInputs = [
            pkgs.gcc
            pkgs.binutils
            pkgs.gdb
            pkgs.clang-tools # for for clang-format
          ];
        };

        packages = {
          default = self'.packages.stdenv-example;
        };

        stdenvMatrix = {
          fs_alloc = {
            inherit stdenvs;
            mkDerivationAttrs = {
              pname = "fs_alloc";
              inherit version;
              src = nix-filter {
                root = ./.;
                include = [
                  "fs_alloc.h"
                  "Makefile"
                ];
              };
              buildPhase = ''
                make fs_alloc.o
              '';
              installPhase = ''
                mkdir -p $out/include
                cp fs_alloc.h $out/include
                mkdir -p $out/lib
                cp fs_alloc.o $out/lib
              '';
              meta = {
                license = lib.licenses.gpl3Plus;
              };
            };
          };

          ll_alloc = {
            inherit stdenvs;
            mkDerivationAttrs = {
              pname = "ll_alloc";
              inherit version;
              src = nix-filter {
                root = ./.;
                include = [
                  "ll_alloc.h"
                  "fs_alloc.h"
                  "Makefile"
                ];
              };
              buildPhase = ''
                make ll_alloc.o
              '';
              installPhase = ''
                mkdir -p $out/include
                cp ll_alloc.h $out/include
                mkdir -p $out/lib
                cp ll_alloc.o $out/lib
              '';
              meta = {
                license = lib.licenses.gpl3Plus;
              };
            };
          };

          example = {
            inherit stdenvs;
            mkDerivationAttrs = {
              pname = "example";
              inherit version;
              src = nix-filter {
                root = ./.;
                include = [
                  "ll_alloc.h"
                  "fs_alloc.h"
                  "example.c"
                  "Makefile"
                ];
              };
              buildPhase = ''
                make example
              '';
              installPhase = ''
                mkdir -p $out/bin
                cp example $out/bin
              '';
              meta = {
                license = lib.licenses.gpl3Plus;
              };
            };
          };
        };

        apps = {
          example = {
            type = "app";
            program = "${self'.packages.example}/bin/example";
          };
        };

        formatter = pkgs.alejandra;
      };
    };
}
