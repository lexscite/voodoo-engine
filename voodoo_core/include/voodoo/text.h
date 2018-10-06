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
// along with Foobar.  If not, see <https://www.gnu.org/licenses/>.

#ifndef VOODOO_TEXT_H_
#define VOODOO_TEXT_H_

#include "component.h"
#include "material.h"
#include "text_mesh.h"

namespace voodoo {
class Font;
class Material;

class Text : public Component {
 public:
  Text(std::string text, std::string vs_path, std::string ps_path,
       std::string texture_path);

  void SetText(const std::string& text) { text_ = text; }

 private:
  virtual bool OnInit() override;
  virtual bool OnUpdate() override;

 private:
  std::string text_;

  std::string vs_path_;
  std::string ps_path_;
  std::string texture_path_;

  std::shared_ptr<TextMesh> mesh_;
  std::shared_ptr<Material> material_;
};
}  // namespace voodoo

#endif