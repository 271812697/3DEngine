/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <array>

#include <OvUI/Widgets/Layout/Columns.h>

#include <OvRendering/Resources/Mesh.h>

#include "OvCore/Resources/Material.h"
#include "OvCore/ECS/Components/AComponent.h"

#define MAX_MATERIAL_COUNT 255

namespace OvCore::ECS { class Actor; }

namespace OvCore::ECS::Components
{
	/**
	* A component that handle a material list, necessary for model rendering
	*/
	class CMaterialRenderer : public AComponent
	{
	public:
		using MaterialList = std::array<OvCore::Resources::Material*, MAX_MATERIAL_COUNT>;
		using MaterialField = std::array<std::array<OvUI::Widgets::AWidget*, 3>, MAX_MATERIAL_COUNT>;

		/**
		* Constructor
		* @param p_owner
		*/
		CMaterialRenderer(ECS::Actor& p_owner);

		/**
		* Returns the name of the component
		*/
		std::string GetName() override;

		/**
		* Fill the material renderer with the given material
		* @param p_material
		*/
		void FillWithMaterial(OvCore::Resources::Material& p_material);

		/**
		* Defines the material to use for the given index
		* @param p_index
		* @param p_material
		*/
		void SetMaterialAtIndex(uint8_t p_index, OvCore::Resources::Material& p_material);

		/**
		* Returns the material to use at index
		* @param p_index
		*/
		OvCore::Resources::Material* GetMaterialAtIndex(uint8_t p_index);

		/**
		* Remove the material at index
		* @param p_index
		*/
		void RemoveMaterialAtIndex(uint8_t p_index);

		/**
		* Remove the material by instance
		* @param p_instance
		*/
		void RemoveMaterialByInstance(OvCore::Resources::Material& p_instance);

		/**
		* Remove every materials
		*/
		void RemoveAllMaterials();

		/**
		* Update the material list by fetching model information
		*/
		void UpdateMaterialList();

		/**
		* Defines an element of the user matrix
		* @param p_row
		* @param p_column
		* @param p_value
		*/
		void SetUserMatrixElement(uint32_t p_row, uint32_t p_column, float p_value);

		/**
		* Returns an element of the user matrix
		* @param p_row
		* @param p_column
		*/
		float GetUserMatrixElement(uint32_t p_row, uint32_t p_column) const;

		/**
		* Returns the user matrix
		*/
		const OvMaths::FMatrix4& GetUserMatrix() const;

		/**
		* Returns the materials
		*/
		const MaterialList& GetMaterials() const;

		/**
		* Serialize the component
		* @param p_doc
		* @param p_node
		*/
		virtual void OnSerialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node) override;

		/**
		* Deserialize the component
		* @param p_doc
		* @param p_node
		*/
		virtual void OnDeserialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node) override;

		/**
		* Defines how the component should be drawn in the inspector
		* @param p_root
		*/
		virtual void OnInspector(OvUI::Internal::WidgetContainer& p_root) override;

	private:
		MaterialList m_materials;
		MaterialField m_materialFields;
		std::array<std::string, MAX_MATERIAL_COUNT> m_materialNames;
		OvMaths::FMatrix4 m_userMatrix;
	};
}