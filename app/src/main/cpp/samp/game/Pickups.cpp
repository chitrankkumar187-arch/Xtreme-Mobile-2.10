//
// Created by x1y2z on 08.11.2023.
//

#include "Pickups.h"
#include "../main.h"
#include "game.h"
#include "net/netgame.h"
#include "../vendor/armhook/patch.h"
#include "World.h"
#include "Pickup.h"
#include "Camera.h"
#include "game/Models/ModelInfo.h"

extern CNetGame* pNetGame;
void CPickups::InjectHooks() {
}

void CPickups::Update() {
    static std::array<CPickup, MAX_NUM_PICKUPS>& aPickUps = *(std::array<CPickup, MAX_NUM_PICKUPS>*)(g_libGTASA + (VER_x32 ? 0x007AFD70 : 0x991AB8));

    auto start = 620 * (CTimer::GetFrameCounter() % 32) / 32;
    auto end   = 620 * (CTimer::GetFrameCounter() % 32 + 1) / 32;
    for (auto i = start; i < end; i++) {
        auto& pickup = aPickUps[i];
        if (pickup.m_nPickupType == PICKUP_NONE)
            continue;

        const int modelId = pickup.m_nModelIndex;
        if (modelId < 0 || modelId >= CModelInfo::NUM_MODEL_INFOS ||
            (modelId >= 19000 && modelId <= 19999)) {
            FLog("[PICKUP] Skipping unsupported engine pickup model %d", modelId);
            pickup.GetRidOfObjects();
            pickup.m_nPickupType = PICKUP_NONE;
            continue;
        }

        if (pickup.m_nFlags.bVisible = pickup.IsVisible()) {
            if (!pickup.m_nFlags.bDisabled && !pickup.m_pObject) {
                pickup.GiveUsAPickUpObject(&pickup.m_pObject, -1);

                if (auto& obj = pickup.m_pObject; obj) {
                    CWorld::Add(obj);
                }
            }
            if (pickup.Update() && pNetGame) {
                CPickupPool *pPickups = pNetGame->GetPickupPool();
                if (pPickups)
                    pPickups->PickedUp(i);
            }
        } else {
            pickup.GetRidOfObjects();
        }
    }
}
