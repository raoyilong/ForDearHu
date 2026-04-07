// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HeartWidget.generated.h"

UCLASS()
class DEARHU_API UHeartWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UHeartWidget(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
    virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heart Settings")
    float HeartSize = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heart Settings")
    FColor HeartColor = FColor::Red;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heart Settings")
    float HeartBeatSpeed = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particle Settings")
    int32 EmitPointCount = 5;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particle Settings")
    int32 ParticlesPerEmit = 5;

private:
    // 爱心相关
    void GenerateHeartPoints();
    TArray<FVector2D> HeartPoints;
    float BeatTime;

    // 粒子效果
    struct FParticle
    {
        FVector2D Position;
        FVector2D Velocity;
        float Lifetime;
        float MaxLifetime;
        FColor Color;
        float Size;
    };

    TArray<FParticle> Particles;
    void SpawnParticles();
    void UpdateParticles(float DeltaTime);
    void DrawParticles(FSlateWindowElementList& OutDrawElements, int32& LayerId, const FGeometry& AllottedGeometry) const;
};