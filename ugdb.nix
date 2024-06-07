{ pkgs }:
pkgs.rustPlatform.buildRustPackage rec {
  pname = "ugdb";
  version = "0.1.11";
  src = fetchGit {
    url = "https://github.com/ftilde/ugdb";
    name = "ugdb";
  };
  cargoSha256 = "sha256-j/bttlfvrbV5VgOuYgA3nZv22385129rHUDs9xyMUEs=";
  nativeBuildInputs = [ pkgs.gdb ];
  patchPhase = ''
    runHook prePatch
    sed -i "575,584d" src/completion/mod.rs
    runHook postPatch
  '';
  postInstall = ''
    cp ${pkgs.lib.getExe' pkgs.gdb "gdb"} $out/bin
  '';
}
