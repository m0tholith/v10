{
  pkgs ? import <nixpkgs> { },
}:

pkgs.mkShell {
  packages = with pkgs; [
    cmake
    glfw
    cglm
    assimp

    # for debugging
    (pkgs.callPackage ./ugdb.nix {})
  ];
}
