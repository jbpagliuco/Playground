#include "CameraComponent.h"

#include "Renderer/Renderer.h"
#include "Renderer/Scene/Scene.h"

#include "Engine/Components/SkyboxComponent.h"
#include "Engine/World/GameObject.h"


namespace playground
{
	void CameraComponent::Deserialize(DeserializationParameterMap& params)
	{
		mCamera.mEnabled = params["enabled"].AsBool();
		mCamera.mFOV = ToRadians(params["fov"].AsFloat());
		mCamera.mNear = params["near"].AsFloat();
		mCamera.mFar = params["far"].AsFloat();

		if (params.HasChild("renderTarget")) {
			mRenderTargetID = RequestAsset(params["renderTarget"].AsFilepath());
			mCamera.mRenderTarget = RenderTarget::Get(mRenderTargetID);
		}
	}

	void CameraComponent::Activate()
	{
		SkyboxComponent* skybox = GetOwner()->GetComponentOfType<SkyboxComponent>();
		if (skybox != nullptr) {
			mCamera.mSkybox = skybox->GetSkybox();
		}

		mCamera.mTransform = *mTransform;
		Scene::Get()->AddCamera(&mCamera);
	}

	void CameraComponent::Deactivate()
	{
		if (mRenderTargetID != INVALID_ASSET_ID) {
			ReleaseAsset(mRenderTargetID);
		}

		mCamera.mEnabled = false;
		Scene::Get()->RemoveCamera(&mCamera);
	}

	void CameraComponent::UpdateLate(float deltaTime)
	{
		mCamera.mTransform = *mTransform;
	}
}