{
  description = "Nix shell for packages required to build this project";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixpkgs-unstable";
    nixos-24-05.url = "github:nixos/nixpkgs?ref=nixos-24.05";
  };

  outputs =
    {
      self,
      nixpkgs,
      nixos-24-05,
    }:
    let
      system = "x86_64-linux";
      pkgs = nixpkgs.legacyPackages.${system};
      pkgs-24-05 = nixos-24-05.legacyPackages.${system};
    in
    {
      devShells.${system}.default = (pkgs.mkShell.override { stdenv = pkgs.clangStdenv; }) {
        packages = with pkgs; [
          glfw
          cglm
          assimp

          # lsp
          clang-tools
          clang

          # compilation
          gnumake
          pkg-config

          # debugging
          valgrind
          renderdoc
          gdb
        ];
      };
    };
}
