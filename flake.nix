{
  description = "Development environment for libansilove";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-24.05";
    nixpkgs-unstable.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = {
    self,
    nixpkgs,
    nixpkgs-unstable,
    flake-utils,
  }:
    flake-utils.lib.eachDefaultSystem (
      system: let
        pkgs = import nixpkgs {inherit system;};
        unstablePkgs = import nixpkgs-unstable { inherit system; };
        version = "1.4.2";
      in {
        packages.default = pkgs.stdenv.mkDerivation {
          pname = "libansilove";
          inherit version;
          src = ./.;
          nativeBuildInputs = [
            pkgs.cmake
            pkgs.pkg-config
          ];
          buildInputs = [
            pkgs.gd
          ];
          cmakeFlags = [
            "-DCMAKE_BUILD_TYPE=Release"
          ];
          meta = with pkgs.lib; {
            description = "Library to convert ANSI and artscene formats to PNG";
            homepage = "https://www.ansilove.org";
            license = licenses.bsd2;
            maintainers = [];
            platforms = platforms.unix;
          };
        };

        devShells.default = pkgs.mkShell {
          packages =
            [
              pkgs.cmake
              pkgs.ninja
              pkgs.pkg-config
              pkgs.gd
              pkgs.emscripten
              unstablePkgs.bun
              pkgs.clang
              pkgs.clang-tools
            ]
            ++ pkgs.lib.optionals (!pkgs.stdenv.isDarwin) [
              pkgs.gdb
            ]
            ++ pkgs.lib.optionals pkgs.stdenv.isDarwin [
              pkgs.lldb
            ];
          shellHook = ''
            export CC=${pkgs.clang}/bin/clang
            export CXX=${pkgs.clang}/bin/clang++
            echo "libansilove dev shell loaded."
          '';
        };
      }
    );
}
