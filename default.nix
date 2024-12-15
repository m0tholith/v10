{
  clangStdenv,
  glfw,
  cglm,
  assimp,
  gnumake,
  pkg-config,
}:
clangStdenv.mkDerivation {
  name = "v10";
  src = ./.;

  strictDeps = true;

  nativeBuildInputs = [
    gnumake
    pkg-config
  ];

  buildInputs = [
    glfw
    cglm
    assimp
  ];

  buildFlags = [
    "CREATE_SO=yes"
  ];

  enableParallelBuilding = true;

  installFlags = [
    "INSTALL_DIR=${placeholder "out"}"
  ];
}
