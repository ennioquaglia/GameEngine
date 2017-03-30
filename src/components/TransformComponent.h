#pragma once
#include "Object.h"
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <map>

namespace Engine {

    class TransformComponent : public Component
	{
        struct TransformData{
            glm::vec3 m_origin,m_scale=glm::vec3(1);
            glm::quat m_rotation;
            glm::mat4 getMatrix();
        };

	public:
        COMPONENT_TYPE_INFORMATION(TransformComponent)

		glm::mat4 matrix, local_matrix;
        std::map<unsigned int, TransformData> localMatrices;
        //glm::vec3 pos, scale;

        glm::vec3 m_origin,m_scale;
        glm::quat m_rotation;


        bool use_matrix=0;
        //Rotation rot;
	public:
		TransformComponent();
		~TransformComponent();
/**/
        virtual glm::mat4 getLocalMatrix(const unsigned int id);
        virtual glm::vec3 getLocalScale(const unsigned int id, bool global = 1);
        virtual glm::vec3 getLocalPosition(const unsigned int id, bool global = 1);
        virtual glm::quat getLocalRotation(const unsigned int id, bool global = 1);

        virtual void setLocalMatrix(const unsigned int id, glm::mat4 matrix);
        virtual void setLocalPosition(const unsigned int id, glm::vec3 v);
        virtual void setLocalScale(const unsigned int id, glm::vec3 v);
        virtual void setLocalRotation(const unsigned int id, glm::quat q);




        virtual void setScale(const glm::vec3 v);
        virtual void setPosition(const glm::vec3 v);
        virtual void setRotation(const glm::quat r);

        virtual glm::vec3 getPosition() const;
        virtual glm::vec3 getScale() const;
        virtual glm::quat getRotation() const;

        virtual glm::mat4 getMatrix(const unsigned int id=Object::render);// = ComponentId::Render);


        void MultMatrix();
		void setMatrix(glm::mat4 m, bool use_scale = 0);

        void serializeXML(std::ostream &stream,unsigned level=0) const;



#if ENGINE_USE_QT_PROPERTY_BROWSER
    virtual void setupProperties(QtVariantPropertyManager *variantManager);
    virtual void updateProperties();
    virtual void updateFromProperties(std::string name,QVariant value,Scene *scene);
#endif
        /*virtual Rotation getLocalRotation(unsigned int id, bool global = 1) {
            mat4 m;
            if (global)m = getMatrix(id);
            else m = getLocalMatrix(id);
            Rotation rot;
            quat q = glm::quat_cast(m);
            q = normalize(q);
            rot.setQuaternion(q);
            return rot;
        }*/
	};

}


