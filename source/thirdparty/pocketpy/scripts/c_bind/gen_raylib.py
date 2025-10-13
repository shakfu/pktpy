import json
from c_bind import Library, set_vmath_converter

with open('../3rd/raylib/parser/output/raylib_api.json') as f:
    data = json.load(f)

lib = Library.from_raylib(data)
set_vmath_converter('Vector2', 'vec2')
set_vmath_converter('Vector3', 'vec3')

lib.build(
    includes=['raylib.h'],
    glue_dir='../src',
    stub_dir='../include/typings'
)
