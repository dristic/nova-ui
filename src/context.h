// Copyright 2017 Dan Ristic

#ifndef SRC_CONTEXT_H_
#define SRC_CONTEXT_H_

#include <string>
#include <memory>

#include "src/animation.h"
#include "src/document.h"
#include "src/types.h"

namespace Chimera {

class Adaptor;
class Renderer;

class Context {
 public:
    Context();
    ~Context();

    void update(double dt);
    void render(float dt);
    void addInputCharacters(std::string characters);
    void setKeyPress(KeyType key);
    void setMousePosition(int x, int y);
    void setScrollDirection(int direction);
    Vec2 getMousePosition();
    void setMouseDown();
    void useAdaptor(std::shared_ptr<Adaptor> adaptor);
    std::unique_ptr<ImageRef> loadImage(std::string name, int& width, int& height);
    unsigned int loadTexture(unsigned int width, unsigned int height, unsigned char* buffer);

    Document document;
    std::unique_ptr<Renderer> renderer;

 private:
    int mMouseX, mMouseY, mScrollDirection;
    std::shared_ptr<Adaptor> mAdaptor;
};

}  // namespace Chimera

#endif  // SRC_CONTEXT_H_
