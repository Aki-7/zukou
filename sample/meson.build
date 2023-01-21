default_vert_glsl = custom_target(
  'default.vertex.h',
  command: [textify, 'default_vertex_shader_source', '@INPUT@', '@OUTPUT@'],
  input: 'default.vert',
  output: 'default.vert.h',
)

color_frag_glsl = custom_target(
  'color.fragment.h',
  command: [textify, 'color_fragment_shader_source', '@INPUT@', '@OUTPUT@'],
  input: 'color.frag',
  output: 'color.fragment.h',
)

texture_frag_glsl = custom_target(
  'texture.fragment.h',
  command: [textify, 'texture_fragment_shader_source', '@INPUT@', '@OUTPUT@'],
  input: 'texture.frag',
  output: 'texture.fragment.h',
)

sphere_geometry_vert_glsl = custom_target(
  'sphere-geometry.vertex.h',
  command: [textify, 'sphere_geometry_vertex_shader_source', '@INPUT@', '@OUTPUT@'],
  input: 'sphere-geometry.vert',
  output: 'sphere-geometry.vert.h',
)

sphere_geometry_geom_glsl = custom_target(
  'sphere-geometry.geometry.h',
  command: [textify, 'sphere_geometry_geometry_shader_source', '@INPUT@', '@OUTPUT@'],
  input: 'sphere-geometry.geom',
  output: 'sphere-geometry.geom.h',
)

sphere_geometry_frag_glsl = custom_target(
  'sphere-geometry.fragment.h',
  command: [textify, 'sphere_geometry_fragment_shader_source', '@INPUT@', '@OUTPUT@'],
  input: 'sphere-geometry.frag',
  output: 'sphere-geometry.frag.h',
)

sphere_bump_vert_glsl = custom_target(
  'sphere-bump.vertex.h',
  command: [textify, 'sphere_bump_vertex_shader_source', '@INPUT@', '@OUTPUT@'],
  input: 'sphere-bump.vert',
  output: 'sphere-bump.vert.h',
)

sphere_bump_frag_glsl = custom_target(
  'sphere-bump.fragment.h',
  command: [textify, 'sphere_bump_fragment_shader_source', '@INPUT@', '@OUTPUT@'],
  input: 'sphere-bump.frag',
  output: 'sphere-bump.frag.h',
)

_samples = {
  'box': [
    'box.cc',
    'cuboid.cc',
    color_frag_glsl,
    default_vert_glsl,
  ],
  'celestial-body': [
    'celestial-body.cc',
    'sphere.cc',
    'jpeg-texture.cc',
    color_frag_glsl,
    texture_frag_glsl,
    default_vert_glsl,
  ],
  'geometry': [
    'geometry-shader.cc',
    'sphere-geometry.cc',
    'jpeg-texture.cc',
    sphere_geometry_vert_glsl,
    sphere_geometry_geom_glsl,
    sphere_geometry_frag_glsl,
  ],
  'bump': [
    'bump-map.cc',
    'sphere-bump.cc',
    'tiff-texture.cc',
    'jpeg-texture.cc',
    sphere_bump_vert_glsl,
    sphere_bump_frag_glsl,
  ]
}

samples = [
  'ray-marching',
]

foreach sample : samples
  frag_glsl = custom_target(
    '@0@.fragment.h'.format(sample),
    command: [textify, '@0@_fragment_shader_source'.format(sample.underscorify()), '@INPUT@', '@OUTPUT@'],
    input: '@0@.frag'.format(sample),
    output: '@0@.fragment.h'.format(sample),
  )

  vert_glsl = custom_target(
    '@0@.vertex.h'.format(sample),
    command: [textify, '@0@_vertex_shader_source'.format(sample.underscorify()), '@INPUT@', '@OUTPUT@'],
    input: '@0@.vert'.format(sample),
    output: '@0@.vertex.h'.format(sample),
  )

  _samples += {
    sample: [
      '@0@.cc'.format(sample),
      frag_glsl,
      vert_glsl,
    ]
  }  
endforeach


_samples_dep = [
  jpeg_dep,
  tiff_dep,
]

foreach name, sources : _samples
  executable(
    'zukou-@0@'.format(name),
    sources,
    install: false,
    dependencies: [ zukou_dep, _samples_dep ],
  )
endforeach
