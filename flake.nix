{
  description = "Development environment for libansilove";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-24.05";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = {
    self,
    nixpkgs,
    flake-utils,
  }:
    flake-utils.lib.eachDefaultSystem (
      system: let
        pkgs = import nixpkgs {inherit system;};
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
            pkgs.libgd
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
          packages = [
            pkgs.cmake
            pkgs.ninja
            pkgs.pkg-config
            pkgs.libgd
            pkgs.clang
            pkgs.clang-tools
            pkgs.gdb
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
