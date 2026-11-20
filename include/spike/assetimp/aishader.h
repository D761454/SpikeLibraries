/*
----------------------------------------------------------------------

BSD 3-Clause License

Copyright (c) 2026, Cayden Andrews

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

----------------------------------------------------------------------
*/

#ifndef __ASSET_IMP_SHADER_H
#define __ASSET_IMP_SHADER_H

/*
----------------------------------------------------------------------

File Summary:
	Holds an imported shader file/program, currently geared towards 
	glsl using an explicit file structure defined below:

----------------------------------------------------------------------

example_shader.glsl
-----------

#version 450 core

// ============================================================================
// SHADER_STAGES
// This file contains multiple shader stages compiled into a single program.
// Each stage is delimited by stage markers below.
// STAGE LIST:
//      VERTEX
//      FRAGMENT
//      GEOMETRY
//      COMPUTE
//      TESS_CONTROL
//      TESS_EVAL
//      MESH
//      TASK
// ============================================================================

// ============================================================================
// STAGE: VERTEX
// ============================================================================
#ifdef VERTEX_STAGE
layout(location = 0) in vec3 _vertexPosition;
layout(location = 1) in vec3 _vertexNormal;
layout(location = 2) in vec2 _vertexTexCoord;

uniform mat4 _transform;
uniform mat4 _view;
uniform mat4 _projection;

out vec2 _fragmentTexCoord;

void main() {
	gl_Position = _projection * _view * _transform * vec4(_vertexPosition, 1.0);
	_fragmentTexCoord = _vertexTexCoord;
}
#endif

// ============================================================================
// STAGE: FRAGMENT
// ============================================================================
#ifdef FRAGMENT_STAGE
in vec2 _fragmentTexCoord;

uniform sampler2D _texture0;
uniform vec4 _colour0;

out vec4 _fragmentColour;

void main() {
	_fragmentColour = texture(_texture0, _fragmentTexCoord) * _colour0;
}
#endif

----------------------------------------------------------------------
*/

#include <vector>

#include "spike/def_aliases.h"
#include "spike/stl_aliases.h"

namespace assimp {
	enum ShaderType_ : u32 {
		ShaderType_vertex = 0,
		ShaderType_geometry = 1,
		ShaderType_fragment = 2,
		ShaderType_tesc = 3,
		ShaderType_hull = ShaderType_tesc,
		ShaderType_tese = 4,
		ShaderType_domain = ShaderType_tese,
		ShaderType_compute = 5,
		ShaderType_mesh = 6,
		ShaderType_task = 7,
		ShaderType_unknown = kMaxU32
	};

	struct ShaderStage {
		ShaderType_ type;
	};

	struct ShaderProgram {
		string name;
		string source;
		std::vector<ShaderStage> shaders;
	};
}

#endif