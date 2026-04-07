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
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heart Settings", meta = (Tooltip = "爱心的大小"))
    float HeartSize = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heart Settings", meta = (Tooltip = "爱心的颜色"))
    FColor HeartColor = FColor::Red;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heart Settings", meta = (Tooltip = "爱心跳动的速度"))
    float HeartBeatSpeed = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heart Settings", meta = (Tooltip = "爱心跳动的幅度"))
    float HeartBeatAmplitude = 0.1f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heart Settings", meta = (Tooltip = "是否显示爱心线条"))
    bool bShowHeartLines = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particle Settings", meta = (Tooltip = "静态粒子的数量"))
    int32 StaticParticleCount = 20;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particle Settings", meta = (Tooltip = "静态粒子的大小"))
    float StaticParticleSize = 3.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particle Settings", meta = (Tooltip = "静态粒子的颜色"))
    FColor StaticParticleColor = FColor::Red;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particle Settings", meta = (Tooltip = "静态粒子光晕的半径，是粒子大小的倍数"))
    float StaticParticleGlowRadius = 3.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particle Settings", meta = (Tooltip = "静态粒子在相邻平均点之间的随机分布范围，0表示不随机，1表示完全随机"))
    float StaticParticleRandomRange = 0.1f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particle Settings", meta = (Tooltip = "动态粒子发射点的数量"))
    int32 EmitPointCount = 5;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particle Settings", meta = (Tooltip = "每个发射点的动态粒子数量"))
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

    struct FStaticParticle
    {
        FVector2D Position;
        FColor Color;
        float Size;
    };

    TArray<FParticle> Particles;
    void SpawnParticles();
    void UpdateParticles(float DeltaTime);
    void DrawParticles(FSlateWindowElementList& OutDrawElements, int32& LayerId, const FGeometry& AllottedGeometry) const;
    
    TArray<FStaticParticle> StaticParticles;
    void GenerateStaticParticles();
    void DrawStaticParticles(FSlateWindowElementList& OutDrawElements, int32& LayerId, const FGeometry& AllottedGeometry) const;
};