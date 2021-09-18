cd bh
cargo build
cd ..

cd calc
cargo build
cd ..

cd dcadj
cargo build
cd ..

cd mean
cargo build
cd ..

cd viewer
cargo build
cd ..

cp bh/target/debug/bh ~/bin
cp calc/target/debug/calc ~/bin
cp dcadj/target/debug/dcadj ~/bin
cp mean/target/debug/mean ~/bin
cp viewer/target/debug/viewer ~/bin
