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

#ifndef VOODOO_COMPONENT_H_
#define VOODOO_COMPONENT_H_

#include "game_object.h"
#include "scene.h"
#include "transform.h"

#include <memory>
#include <string>

namespace voodoo {
class Transform;
class Scene;

class Component : public std::enable_shared_from_this<Component> {
 public:
  virtual bool Init(std::shared_ptr<GameObject> game_object) final;
  virtual bool Update();

  std::string GetName();

  std::shared_ptr<Scene> GetScene();
  std::shared_ptr<GameObject> GetGameObject();
  std::shared_ptr<Transform> GetTransform();

 private:
  virtual bool OnInit();
  virtual bool OnUpdate();

 private:
  std::shared_ptr<GameObject> game_object_;
};
}  // namespace voodoo

#endif