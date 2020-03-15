#pragma once
#include <Windows.h>
#include <iostream>

#include "imgui\imgui.h"
#include "imgui\imgui_internal.h"


#include "Sdk.h"
#include "Renderer.h"
#include "Core.h"
#include "Engine.h"
#include "FeatureManager.h"

//Features
#include "ResourceInformation.h"
#include "ESP.h"
#include "MinimapText.h"

Core::Core()
{
	//Register Features here
	FeatureManager::Get()->registerFeature(new ResourceInformation());
	FeatureManager::Get()->registerFeature(new ESP());
	FeatureManager::Get()->registerFeature(new MinimapText());

	FeatureManager::Get()->OnInitialise();
}

void createPlayerTreeNode(Player* player, int playerIndex)
{
	ImGui::PushStyleColor(ImGuiCol_Text, Engine::Get()->GetPlayerColorImGUI(*player->color));

	std::string playerText = "Player " + std::to_string(playerIndex);
	if (ImGui::TreeNode(player->name))
	{
		ImGui::Text("Player %p", player);
		FeatureManager::Get()->OnMenuPlayerTreenode(player, playerIndex);
		if (ImGui::TreeNode("Units"))
		{
			int buildingCount = 0;
			int infantryCount = 0;
			int calavaryCount = 0;
			for (int i = 0; i < player->pObjectManager->Count; i++)
			{
				Unit* unit = player->pObjectManager->units[i];
				if (!unit) { continue; }

				if (unit->pOwner == player)
				{
					ImGui::Text("%p", unit);
					ImGui::SameLine();
					ImGui::Text("%s", unit->pUnitData->name);

			/*		if (unit->pUnitData->Class == (int16_t)EnumUnitDataClass::Building)
					{
						buildingCount++;
					}
					if (unit->pUnitData->Class == (int16_t)EnumUnitDataClass::Infantry)
					{
						infantryCount++;
					}
					if (unit->pUnitData->Class == (int16_t)EnumUnitDataClass::Cavalry)
					{
						calavaryCount++;
					}*/
				}
			}
			ImGui::Text("Buildings %.d", buildingCount);
			ImGui::Text("Infantry %.d", infantryCount);
			ImGui::Text("Cavalry %.d", calavaryCount);
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
	ImGui::PopStyleColor();
}


void Core::OnPresent()
{
	__try
	{
		MainScreen* mainScreen = Engine::Get()->GetMainScreen();
		if (!mainScreen)
		{
			return;
		}

		World* world = Engine::Get()->GetWorld();


		PlayerArray* playerArray = world->pPlayerArray;
		if (!playerArray)
		{
			return;
		}
		int totalPlayers = Engine::Get()->GetTotalPlayers();

		static bool openOverlay = true;
		if (GetAsyncKeyState(VK_INSERT) & 1) { openOverlay = !openOverlay; }

		Renderer::Get()->BeginScene();
		FeatureManager::Get()->OnDraw();
		Player* gaiaPlayer = *(Player**)(playerArray);

		if (gaiaPlayer)
		{
			for (int i = 0; i < gaiaPlayer->pObjectManager->Count; i++)
			{
				Unit* unit = gaiaPlayer->pObjectManager->units[i];
				if (!unit)
				{
					continue;
				}
				FeatureManager::Get()->OnNeutralUnit(unit);
			}
		}

		for (int i = 1; i <= totalPlayers; i++)
		{
			Player* player = playerArray->playerData[i].player;
			if (!player)
			{
				continue;
			}
			if (skipLocalplayer && player == Engine::Get()->GetLocalPlayer()) 
			{
				continue;
			}
			FeatureManager::Get()->OnPlayerIteration(player, i);
			for (int j = 0; j < player->pObjectManager->Count; j++)
			{
				Unit* unit = player->pObjectManager->units[j];
				if (!unit)
				{
					continue;
				}
				FeatureManager::Get()->OnUnitIteration(unit, player, i);
			}
		}

		Renderer::Get()->EndScene();

		ImGui::SetNextWindowBgAlpha(0.35f);
		if (openOverlay)
		{
			__try
			{
				if (ImGui::Begin("Age of Empires 2 DE", &openOverlay, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
				{
					ImGui::Text("World %p", world);
					ImGui::Text("Map %p tilesize %d", world->pMap, world->pMap->GetTileSize());
					ImGui::Text("Localplayer %p", Engine::Get()->GetLocalPlayer());
					ImGui::Text("PlayerArray %p", playerArray);
					ImGui::Text("totalPlayers %d", totalPlayers);
					ImGui::Text("ScreenPos %f %f %f", mainScreen->pGameScreen->pMainView->ScreenPosX, mainScreen->pGameScreen->pMainView->ScreenPosY, mainScreen->pGameScreen->pMainView->ScreenPosZ);
					for (int i = 0; i <= totalPlayers; i++)
					{
						Player* currentPlayer = playerArray->playerData[i].player;
						if (skipLocalplayer && currentPlayer == Engine::Get()->GetLocalPlayer())
						{
							continue;
						}
						createPlayerTreeNode(currentPlayer, i);
					}
					FeatureManager::Get()->OnMenuMainWindow();
					ImGui::Checkbox("Skip localplayer", &skipLocalplayer);
				}
			}
			__finally
			{
				ImGui::End();
			}
		}
	}
	__except (1)
	{
		if (Renderer::Get()->inFrame)
		{
			Renderer::Get()->EndScene();
		}
	}
}