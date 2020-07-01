#pragma once

#include <functional>
#include <Material.h>
#include "ox/oxygine.hpp"
#include "ox/Sprite.hpp"
#include "CustomSprite.h"
#include "CustomRenderDelegate.h"

namespace render
{
	//Uses for sprites with animations
	class GrassSprite : public CustomSprite
	{
	public:
		GrassSprite();
		~GrassSprite();

		void init(const render::TextureMap & grassMask, oxygine::RectF srcRect, oxygine::RectF destRect,
				oxygine::vertexPNT2* grassBladesData, std::uint32_t grassBladesCount);

		void sortBladesByAngle(float angle);

		void update(const oxygine::UpdateState& us) override;
		void render(const render::RenderState& rs) override;

	protected:
		void initGrassBlades(oxygine::vertexPNT2* grassBladesData, std::uint32_t grassBladesCount, oxygine::RectF destRect);

		void flipX() override;
		void flipY() override;

	private:
		std::vector<oxygine::vertexPNT2> _grassBlades;
		CustomMaterial* _noiseMaterial;
	};
}