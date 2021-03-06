// This file is part of Voodoo Engine.
//
// Voodoo Engine is free software : you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Voodoo Engine is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Voodoo Engine.  If not, see <https://www.gnu.org/licenses/>.

#include "../include/voodoo/text.h"

namespace voodoo {
string Text::GetText() {
  return text_;
}

void Text::SetText(const string& text) {
  text_ = text;
}

sptr<Font> Text::GetFont() {
  return font_;
}

void Text::SetFont(sptr<Font> font) {
  font_ = font;
}

sptr<Material> Text::GetMaterial() {
  return material_;
}

void Text::SetMaterial(sptr<Material> material) {
  material_ = material;
}

void Text::Start() {
  mesh_ = GenerateMesh();
  renderer_ = GetComponent<Renderer>();
  renderer_->SetMesh(mesh_);
  renderer_->SetMaterial(material_);
}

sptr<Mesh> Text::GenerateMesh() {
  vector<vertex_ptn> vs;
  float offset = 0;

  for (auto& c : text_) {
    auto c_data = font_->GetCharData(c);
    auto v = GenerateChar(c_data, offset);
    vs.insert(vs.begin(), v.begin(), v.end());
    offset += c_data.width;
  }

  return std::make_shared<Mesh>(vs);
}

vector<vertex_ptn> Text::GenerateChar(Font::CharData c, const float& offset) {
  float width = font_->GetWidth();
  float height = font_->GetHeight();

  float r = c.x / width;
  float l = (c.x + c.width) / width;
  float b = c.y / height;
  float t = (c.y + c.height) / height;

  auto lt = vertex_ptn(vec3f(-offset, 0, 0),
                       vec2f(l, t),
                       vec3f(0.0f, 0.0f, 1.0f));

  auto rt = vertex_ptn(vec3f(-offset + c.width, 0, 0),
                       vec2f(r, t),
                       vec3f(0.0f, 0.0f, 1.0f));

  auto lb = vertex_ptn(vec3f(-offset, c.height, 0),
                       vec2f(l, b),
                       vec3f(0.0f, 0.0f, 1.0f));

  auto rb = vertex_ptn(vec3f(-offset + c.width, c.height, 0),
                       vec2f(r, b),
                       vec3f(0.0f, 0.0f, 1.0f));

  return vector<vertex_ptn>({lt, rb, lb, lt, rt, rb});
}
}  // namespace voodoo