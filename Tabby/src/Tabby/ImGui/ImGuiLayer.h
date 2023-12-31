#pragma once

#include "Tabby/Core/Layer.h"

#include "Tabby/Events/ApplicationEvent.h"
#include "Tabby/Events/KeyEvent.h"
#include "Tabby/Events/MouseEvent.h"

namespace Tabby {

	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event& e) override;

		void Begin();
		void End();

		void BlockEvents(bool block) { m_BlockEvents = block; }
		
		void SetDarkThemeColors();

		uint32_t GetActiveWidgetID() const;
	private:
		bool m_BlockEvents = true;
	};

}
