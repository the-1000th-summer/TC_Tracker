#pragma once
#include "ManagedObject.h"
#include "../TTCore/Core.h"

using namespace System;
namespace myCLI {
    public ref class Entity : public ManagedObject<TTCore::Entity> {
    public:
        
        Entity(String^ name, float xPos, float yPos);
        void Move(float deltaX, float deltaY);
        property float XPosition {
        public:
            float get() {
                return m_Instance->GetXPosition();
            }
        private:
            void set(float value) {

            }
        }

        property float YPosition {
        public:
            float get() {
                return m_Instance->GetYPosition();
            }
        private:
            void set(float value) {

            }
        }

    };
}

