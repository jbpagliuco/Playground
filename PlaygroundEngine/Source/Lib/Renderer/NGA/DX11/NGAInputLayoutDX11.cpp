#include "NGA/NGAInputLayout.h"

#if CORE_RENDER_API(DX11)

#include "NGA/NGAShader.h"
#include "NGACoreInternalDX11.h"

namespace playground
{
	NGA_GPU_CLASS_IMPLEMENT(NGAInputLayout);

	bool NGAInputLayout::Construct(const NGAVertexFormatDesc &vertexFormatDesc, const NGAShader &vertexShader)
	{
		CORE_ASSERT_RETURN_VALUE(!IsConstructed(), false);

		std::vector<D3D11_INPUT_ELEMENT_DESC> elementDescs;
		for (auto &attr : vertexFormatDesc.mAttributes) {
			D3D11_INPUT_ELEMENT_DESC element;
			element.InputSlot = 0;
			element.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			element.InstanceDataStepRate = 0;

			element.Format = NGAFormatToDXGI(attr.mFormat);
			element.SemanticName = GetSemanticName(attr.mSemanticType);
			element.SemanticIndex = attr.mSemanticIndex;
			element.AlignedByteOffset = attr.mOffset;

			elementDescs.push_back(element);
		}

		elementDescs[0].AlignedByteOffset = 0;

		HRESULT hr = NgaDx11State.mDevice->CreateInputLayout(&(elementDescs[0]), (UINT)elementDescs.size(), vertexShader.GetBytecode(), vertexShader.GetBytecodeSize(), &mInputLayout);
		CORE_ASSERT_RETURN_VALUE(SUCCEEDED(hr), false, "ID3D11Device::CreateInputLayout failed with HRESULT %X", hr);

		return true;
	}

	void NGAInputLayout::Destruct()
	{
		COM_SAFE_RELEASE(mInputLayout);
	}

	bool NGAInputLayout::IsConstructed()const
	{
		return mInputLayout != nullptr;
	}
}

#endif