#pragma once

//新的枚举宏enum macro,这样这个enum可以被使用在蓝图,作为蓝图的一种类型
UENUM(BlueprintType)
enum class ETurningInPlace : uint8 //同一单位
{//是一些转身的状态来的,
	ETIP_Left UMETA(DisplayName = "Turning Left"),//?????????
	ETIP_Right UMETA(DisplayName = "Turning Right"),
	ETIP_NotTurning UMETA(DisplayName = "Not Turning"),
	ETIP_MAX UMETA(DisplayName = "DefaultMAX")
};
