#pragma once

//�µ�ö�ٺ�enum macro,�������enum���Ա�ʹ������ͼ,��Ϊ��ͼ��һ������
UENUM(BlueprintType)
enum class ETurningInPlace : uint8 //ͬһ��λ
{//��һЩת���״̬����,
	ETIP_Left UMETA(DisplayName = "Turning Left"),//?????????
	ETIP_Right UMETA(DisplayName = "Turning Right"),
	ETIP_NotTurning UMETA(DisplayName = "Not Turning"),
	ETIP_MAX UMETA(DisplayName = "DefaultMAX")
};
