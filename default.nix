{
  clangStdenv,
  glfw,
  cglm,
  assimp,
  gnumake,
}:
clangStdenv.mkDerivation {
  name = "v10";
  src = ./.;

  buildInputs = [
    glfw
    cglm
    assimp

    gnumake
  ];

  buildPhase = ''
    make CREATE_SO=yes -j8
  '';
  installPhase = ''
    make INSTALL_DIR="$out" install
  '';
}
