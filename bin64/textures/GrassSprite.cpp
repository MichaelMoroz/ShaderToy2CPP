#include "GrassSprite.h"
#include "../assets/ResourceManager.h"
#include "../assets/ShaderManager.h"

namespace render
{
	GrassSprite::GrassSprite()
	{
	}

	GrassSprite::~GrassSprite()
	{
	}

	void GrassSprite::init(const render::TextureMap & grassMask, oxygine::RectF srcRect, oxygine::RectF destRect,
		oxygine::vertexPNT2 * grassBladesData, std::uint32_t grassBladesCount)
	{
		_material = new CustomMaterial;
		_material->_base = nullptr;
		_material->_flags = CustomUberShaderProgram::ENABLE_GEOMETRY_SHADER;
		_material->_primitiveType = oxygine::IVideoDriver::PT_POINTS;

		_material->setUberShaderProgram(RenderPass::GBuffer, assets::ShaderManager::Get()->getUberShader("shaders/Grass.glsl"));

		_srcRect = srcRect;

		_noiseMaterial = new CustomMaterial;
		_noiseMaterial->_base = grassMask._texture;
		_noiseMaterial->_flags = 0;

		_noiseMaterial->setUberShaderProgram(RenderPass::GBuffer_Prepass, assets::ShaderManager::Get()->getUberShader("shaders/GrassNoise.glsl"));

		setWidth(destRect.size.x);
		setHeight(destRect.size.y);

		initGrassBlades(grassBladesData, grassBladesCount, destRect);
	}

	void GrassSprite::sortBladesByAngle(float angle)
	{
		oxygine::AffineTransform rotationTransform = oxygine::AffineTransform::getIdentity();
		rotationTransform.rotate(angle);

		auto sortPredicate = [&](const oxygine::vertexPNT2& a, const oxygine::vertexPNT2& b)
		{
			oxygine::Vector2 localPositionA{ a.x, a.y };
			oxygine::Vector2 localPositionB{ b.x, b.y };

			oxygine::Vector2 worldPositionA = rotationTransform.transform(localPositionA);
			oxygine::Vector2 worldPositionB = rotationTransform.transform(localPositionB);

			return worldPositionA.y < worldPositionB.y;
		};
		std::sort(_grassBlades.begin(), _grassBlades.end(), sortPredicate);
	}

	void GrassSprite::initGrassBlades(oxygine::vertexPNT2 * grassBladesData, std::uint32_t grassBladesCount, oxygine::RectF destRect)
	{
		_grassBlades.reserve(grassBladesCount);

		oxygine::spNativeTexture maskTexture = _noiseMaterial->_base;

		oxygine::Vector2 bladesOffset = getSize() / 2.0f;

		int Width = maskTexture->getWidth();
		int Height = maskTexture->getHeight();

		oxygine::Rect textureRect;
		textureRect.pos.x = _srcRect.pos.x * Width;
		textureRect.pos.y = _srcRect.pos.y * Height;
		textureRect.size.x = _srcRect.size.x * Width;
		textureRect.size.y = _srcRect.size.y * Height;

		oxygine::Rect lockRect;
		lockRect.pos.x = (textureRect.size.x < 0.0f) ? textureRect.pos.x + textureRect.size.x : textureRect.pos.x;
		lockRect.pos.y = (textureRect.size.y < 0.0f) ? textureRect.pos.y + textureRect.size.y : textureRect.pos.y;
		lockRect.size.x = std::fabsf(textureRect.size.x);
		lockRect.size.y = std::fabsf(textureRect.size.y);

		oxygine::ImageData data = maskTexture->lock(oxygine::NativeTexture::lock_read, &lockRect);
		for (int i = 0; i < grassBladesCount; i++)
		{
			oxygine::vertexPNT2 vertex = grassBladesData[i];

			oxygine::Vector2 bladeSize = { 12.0f, 1.0f }; //TODO
			oxygine::Vector2 globalVertexPosition = { vertex.x, vertex.y };
			oxygine::Vector2 localVertexPosition = globalVertexPosition - destRect.pos;

			std::uint32_t alpha = 0;
			for (std::uint8_t t = 0; t < 4; t++)
			{
				std::uint8_t x = t % 2;
				std::uint8_t y = t / 2;

				oxygine::Vector2 alignedPosition = localVertexPosition + bladeSize.mult(oxygine::Vector2(x, y));
				oxygine::Vector2 texelCoords = alignedPosition.div(_spriteSize);
				oxygine::Vector2 fragmentCoords = texelCoords.mult(textureRect.size);

				oxygine::Point pixelCoords = fragmentCoords.cast<oxygine::Point>();
				if (pixelCoords.x < 0) {
					pixelCoords.x = std::fmaxf(lockRect.size.x + pixelCoords.x, 0.0f);
				}
				if (pixelCoords.y < 0) {
					pixelCoords.y = std::fmaxf(lockRect.size.y + pixelCoords.y, 0.0f);
				}

				alpha += *((std::uint8_t*)data.getPixelPtr(pixelCoords.x, pixelCoords.y) + 3);
			}
			alpha /= 4;
			if (alpha >= 192) 
			{
				vertex.x = localVertexPosition.x - bladesOffset.x;
				vertex.y = localVertexPosition.y - bladesOffset.y;

				_grassBlades.push_back(vertex);
			}
		}
		maskTexture->unlock();
	}

	void GrassSprite::flipX() 
	{
		//empty
	}

	void GrassSprite::flipY() 
	{
		std::reverse(_grassBlades.begin(), _grassBlades.end());
	}

	void GrassSprite::update(const oxygine::UpdateState & us)
	{
	}

	void GrassSprite::render(const render::RenderState & rs)
	{
		if (rs.passID == RenderPass::GBuffer_Prepass) 
		{
			_noiseMaterial->apply(rs.passID);
			_noiseMaterial->render(rs.transform, _srcRect, _dstRect);
		}
		else 
		{
			_material->apply(rs.passID, rs.flags);
			_material->render(rs.transform, _grassBlades.data(), _grassBlades.size());
		}
	}
}
