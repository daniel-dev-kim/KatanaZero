#include "pch.h"
#include "BossIntro.h"
#include "SpriteManagerComponent.h"
#include "TextureComponent.h"

#include "Engine.h"
#include "AudioManager.h"
#include "Random.h"
#include "TextureManager.h"
BossIntro::BossIntro(ObjectManager* owner, ObjectType type, glm::vec2 portalPos) : Object(owner, type), portalPos(portalPos)
{
	portal = std::make_shared<SpriteManagerComponent>("Resource/Texture/Effect/Boss/Portal/portal.txt");
	portalCover = std::make_shared<SpriteManagerComponent>("Resource/Texture/Effect/Boss/Portal/portal.txt");
	portalMask = std::make_shared<TextureComponent>("portalMask", "Resource/Texture/Effect/Boss/portal/mask.png");

	
	hand = std::make_shared<SpriteManagerComponent>("Resource/Texture/Object/Boss/obj_hand/obj_hand.txt");
	handPos = portalPos + glm::vec2{-7, -handLength };
	currState = HandIntroState::Wait;
	seed = Engine::GetInstance().GetRandom()->get(0.0, 1.0);

}

void BossIntro::Update(double dt)
{
	Object::Update(dt);

	switch (currState)
	{
	case HandIntroState::Wait:
	{
		timer += dt;
		if (timer > 0.)
		{
			timer = 0.;
			portal->PlaySprite("portal_open");
			portalCover->PlaySprite("portal_front");
			hand->PlaySprite("arm_intro_begin");
			currState = HandIntroState::PortalOpen;
			Engine::GetInstance().GetAudioManager()->StartSound("sound_boss_arm_portal_01");

		}
	} break;
	case HandIntroState::PortalOpen:
	{
		portal->Update(dt);

		if (portal->IsDone())
		{
			timer += dt;
		}
		if (timer > 0.2)
		{
			timer = 0.;
			portal->PlaySprite("portal_back");
			currState = HandIntroState::HandCommingOut;
		}
	} break;
	case HandIntroState::HandCommingOut:
	{
		afterimageTimer += dt;
		portal->Update(dt);
		portalCover->Update(dt);
		handPos.y += dt * 2000;
		if (handPos.y > portalPos.y + handLength)
		{
			timer += dt;
			handPos.y = portalPos.y + handLength;
		}
		if (timer > 0.2f)
		{
			timer = 0.f;
			hand->PlaySprite("arm_intro");
			Engine::GetInstance().GetAudioManager()->StartSound("sound_boss_arm_break_0" + std::to_string(Engine::GetInstance().GetRandom()->get(1, 2)));
			currState = HandIntroState::HandCracked;
			
		}
	} break;
	case HandIntroState::HandCracked:
	{
		afterimageTimer += dt;

		portal->Update(dt);
		portalCover->Update(dt);
		hand->Update(dt);
		
		if (!hand->IsDone())
		{
			handPos.y += 39 * dt;
		}
		else if (hand->IsDone())
		{
			if (glitch == 0)
			{
				Engine::GetInstance().GetAudioManager()->StartSound("sound_ui_screen_glitch_01");
				glitch = 2;
			}
			timer += dt;
		}

		if (!glitchSwitch && timer > 0.1f)
		{
			glitchSwitch = true;
			seed += 1.f;
		}

		if (timer > 0.2f)
		{
			glitch = 0;
			timer = 0.f;
			currState = HandIntroState::HandGoingBack;
		}
	} break;
	case HandIntroState::HandGoingBack:
	{
		afterimageTimer += dt;

		portal->Update(dt);
		portalCover->Update(dt);
		handPos.y -= dt * 2000;
		if (handPos.y < portalPos.y - handLength - 50.)
		{
			handPos.y = portalPos.y - handLength - 50.;
			portal->PlaySprite("portal_close");
			currState = HandIntroState::PortalClose;
			Engine::GetInstance().GetAudioManager()->StartSound("sound_boss_arm_portal_02");

		}
	} break;
	case HandIntroState::PortalClose:
	{
		portal->Update(dt);
		if (portal->IsDone())
		{
			SetDead();
		}
	} break;
	}

	if (afterimageTimer > 0.05)
	{
		afterimageTimer = 0.;
		Afterimage afterImage{};
		glm::vec2 newHandPos = { handPos.x, Engine::GetInstance().GetWindowSize().bottom - handPos.y };
		glm::mat4 handMat = glm::translate(glm::mat4(1.0), glm::vec3(newHandPos, 0.0));
		handMat = glm::rotate(handMat, -(float)PI / 2.f, glm::vec3(0.f, 0.f, 1.f));
		handMat = glm::scale(handMat, glm::vec3(2.2, 2.2, 1.0));

		afterImage.modelMat = handMat;
		afterImage.texture = hand->GetCurrentSprite()->GetCurrentData();
		afterImage.lifetime = 0.3;
		afterImage.info.color = glm::vec4(0.36, 0.0, 0.40, 0.8);
		afterimages.push_back(afterImage);
	}

	for (auto& afterimage : afterimages)
	{
		afterimage.lifetime -= dt;
		afterimage.info.color.a -= dt * 2;
	}

	afterimages.remove_if([](const auto& img) { return img.lifetime <= 0.0f; });
}

void BossIntro::Render()
{
	glm::vec2 newPortalPos = { portalPos.x, Engine::GetInstance().GetWindowSize().bottom - portalPos.y };

	glm::mat4 portalMat = glm::translate(glm::mat4(1.0), glm::vec3(newPortalPos, 0.0));
	portalMat = glm::rotate(portalMat, (float)PI / 2.f, glm::vec3(0.f, 0.f, 1.f));
	portalMat = glm::scale(portalMat, glm::vec3(2.0, 2.0, 1.0));


	switch (currState)
	{

	case HandIntroState::HandCommingOut:
	case HandIntroState::HandCracked:
	case HandIntroState::HandGoingBack:
	{
		portal->Render(portalMat);

		glm::vec2 newHandPos = { handPos.x, Engine::GetInstance().GetWindowSize().bottom - handPos.y };
		glm::mat4 handMat = glm::translate(glm::mat4(1.0), glm::vec3(newHandPos, 0.0));
		handMat = glm::rotate(handMat, -(float)PI / 2.f, glm::vec3(0.f, 0.f, 1.f));
		handMat = glm::scale(handMat, glm::vec3(2.2, 2.2, 1.0));
		
		RenderInfo info{};
		info.uEnableObjectGlitch = glitch;
		info.uObjectGlitchSeed = seed;
		info.useMask = true;
		info.mask = Engine::GetInstance().GetTextureManager()->GetTexture(portalMask->GetKey());
		newPortalPos = { portalPos.x, Engine::GetInstance().GetWindowSize().bottom - portalPos.y + 11. };
		portalMat = glm::translate(glm::mat4(1.0), glm::vec3(newPortalPos, 0.0));
		portalMat = glm::rotate(portalMat, (float)PI / 2.f, glm::vec3(0.f, 0.f, 1.f));

		glm::mat4 portalMaskMat = glm::scale(portalMat, glm::vec3(40.0, 52.0, 1.0));
		info.maskMat = portalMaskMat;
		hand->Render(handMat, info);

		portalMat = glm::scale(portalMat, glm::vec3(2.0, 2.0, 1.0));
		portalCover->Render(portalMat);
	}break;
	case HandIntroState::PortalClose:
	case HandIntroState::PortalOpen:
	{
		portal->Render(portalMat);
	}break;
	}
}
