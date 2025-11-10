BRANCH=v2.1.3

function get_pocketpy() {
  cd source/thirdparty && \
  git clone -b ${BRANCH} --depth=1 --recursive https://github.com/pocketpy/pocketpy.git && \
  rm -rf pocketpy/.git
}

if [[ -d "source/thirdparty/pocketpy" ]]; then
  echo "Directory exists."
  mv source/thirdparty/pocketpy source/thirdparty/pocketpy.old
  get_pocketpy
else
  echo "Directory does not exist."
  get_pocketpy
fi
