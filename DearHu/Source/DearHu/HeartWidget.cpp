// Copyright Epic Games, Inc. All Rights Reserved.

#include "HeartWidget.h"
#include "Slate/SceneViewport.h"
#include "Widgets/SWidget.h"
#include "Logging/LogMacros.h"

UHeartWidget::UHeartWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    BeatTime = 0.0f;
}

void UHeartWidget::NativeConstruct()
{
    Super::NativeConstruct();
    GenerateHeartPoints();
    SpawnParticles();
}

void UHeartWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    
    BeatTime += InDeltaTime * HeartBeatSpeed;
    UpdateParticles(InDeltaTime);
    
    // 定期生成新粒子
    if (FMath::Sin(BeatTime * 3.14159f * 2.0f) > 0.8f)
    {
        SpawnParticles();
    }
}

int32 UHeartWidget::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
    int32 CurrentLayerId = LayerId;
    
    try
    {
        // 计算爱心的缩放和位置
        float BeatScale = 1.0f + 0.1f * FMath::Sin(BeatTime * 3.14159f * 2.0f);
        FVector2D HeartCenter = AllottedGeometry.GetLocalSize() / 2.0f;
        
        // 绘制爱心
        if (HeartPoints.Num() > 0)
        {
            TArray<FVector2D> ScaledPoints;
            for (const FVector2D& Point : HeartPoints)
            {
                FVector2D ScaledPoint = Point * HeartSize * BeatScale + HeartCenter;
                ScaledPoints.Add(ScaledPoint);
            }
            
            // 创建爱心路径（使用Lines）
            if (ScaledPoints.Num() > 1)
            {
                FSlateDrawElement::MakeLines(OutDrawElements, CurrentLayerId++, AllottedGeometry.ToPaintGeometry(FVector2f(AllottedGeometry.GetLocalSize()), FSlateLayoutTransform()), ScaledPoints, ESlateDrawEffect::None, HeartColor, true, 2.0f);
            }
        }
        
        // 绘制粒子
        DrawParticles(OutDrawElements, CurrentLayerId, AllottedGeometry);
    }
    catch (...) {}
    
    return CurrentLayerId;
}

void UHeartWidget::GenerateHeartPoints()
{
    HeartPoints.Empty();
    
    // 使用参数方程生成爱心形状
    const int32 NumPoints = 100;
    for (int32 i = 0; i < NumPoints; i++)
    {
        float t = (float)i / (float)(NumPoints - 1) * 3.14159f * 2.0f;
        float x = 16.0f * FMath::Sin(t) * FMath::Sin(t) * FMath::Sin(t);
        float y = 13.0f * FMath::Cos(t) - 5.0f * FMath::Cos(2.0f * t) - 2.0f * FMath::Cos(3.0f * t) - FMath::Cos(4.0f * t);
        
        // 缩放并调整位置
        FVector2D Point(x / 16.0f, -y / 16.0f);
        HeartPoints.Add(Point);
    }
}

void UHeartWidget::SpawnParticles()
{
    // 确保EmitPointCount和ParticlesPerEmit为正数
    int32 SafeEmitPointCount = FMath::Max(1, EmitPointCount);
    int32 SafeParticlesPerEmit = FMath::Max(1, ParticlesPerEmit);
    
    // 生成粒子发射点
    for (int32 i = 0; i < SafeEmitPointCount; i++)
    {
        // 从爱心边缘随机选择一个点作为粒子起始位置
        FVector2D EmitPosition;
        if (HeartPoints.Num() > 0)
        {
            int32 RandomIndex = FMath::RandRange(0, HeartPoints.Num() - 1);
            EmitPosition = HeartPoints[RandomIndex] * HeartSize;
        }
        else
        {
            EmitPosition = FVector2D(0, 0);
        }
        
        // 从每个发射点发射多个粒子
        for (int32 j = 0; j < SafeParticlesPerEmit; j++)
        {
            FParticle Particle;
            Particle.Position = EmitPosition;
            
            // 生成随机速度，让粒子向外扩散
            float Angle = FMath::FRandRange(0.0f, 3.14159f * 2.0f);
            float Speed = FMath::FRandRange(50.0f, 150.0f);
            Particle.Velocity = FVector2D(FMath::Cos(Angle) * Speed, FMath::Sin(Angle) * Speed);
            
            Particle.Lifetime = 0.0f;
            Particle.MaxLifetime = FMath::FRandRange(1.0f, 2.0f);
            FColor Color = FColor::Red;
            Color.A = (uint8)FMath::FRandRange(100.0f, 255.0f);
            Particle.Color = Color;
            Particle.Size = FMath::FRandRange(2.0f, 5.0f);
            Particles.Add(Particle);
        }
    }
}

void UHeartWidget::UpdateParticles(float DeltaTime)
{
    for (int32 i = Particles.Num() - 1; i >= 0; i--)
    {
        FParticle& Particle = Particles[i];
        Particle.Lifetime += DeltaTime;
        
        if (Particle.Lifetime >= Particle.MaxLifetime)
        {
            Particles.RemoveAt(i);
        }
        else
        {
            // 更新粒子位置
            Particle.Position += Particle.Velocity * DeltaTime;
            
            // 应用重力
            Particle.Velocity.Y += 50.0f * DeltaTime;
            
            // 减速
            Particle.Velocity *= 0.98f;
        }
    }
}

void UHeartWidget::DrawParticles(FSlateWindowElementList& OutDrawElements, int32& LayerId, const FGeometry& AllottedGeometry) const
{
    try
    {
        UE_LOG(LogTemp, Log, TEXT("[HeartWidget] DrawParticles started, Particles.Num(): %d"), Particles.Num());
        
        FVector2D HeartCenter = AllottedGeometry.GetLocalSize() / 2.0f;
        UE_LOG(LogTemp, Log, TEXT("[HeartWidget] HeartCenter: %f, %f"), HeartCenter.X, HeartCenter.Y);
        
        // 安全检查：确保Particles数组有效
        if (Particles.Num() == 0)
        {
            UE_LOG(LogTemp, Log, TEXT("[HeartWidget] No particles to draw"));
            return;
        }
        
        int32 ParticleCount = 0;
        for (const FParticle& Particle : Particles)
        {
            // 计算粒子在Widget中的位置
            FVector2D ParticlePosition = Particle.Position + HeartCenter;
            
            // 计算粒子大小和透明度
            float Size = Particle.Size * (1.0f - Particle.Lifetime / Particle.MaxLifetime);
            if (Size <= 0.0f || Size > 100.0f)
            {
                continue; // 限制大小范围
            }
            
            // 确保位置有效
            if (ParticlePosition.X < -10000 || ParticlePosition.X > 10000 ||
                ParticlePosition.Y < -10000 || ParticlePosition.Y > 10000)
            {
                continue;
            }
            
            FColor ParticleColor = Particle.Color;
            ParticleColor.A = (uint8)FMath::Clamp((1.0f - Particle.Lifetime / Particle.MaxLifetime) * 255, 0.0f, 255.0f);
            
            // 绘制粒子（使用Box）
            FVector2D Position = ParticlePosition - FVector2D(Size / 2.0f, Size / 2.0f);
            FVector2D BoxSize = FVector2D(Size, Size);
            
            // 直接使用MakeLines来绘制粒子，避免MakeBox的崩溃问题
            TArray<FVector2D> ParticlePoints;
            ParticlePoints.Add(Position);
            ParticlePoints.Add(Position + BoxSize);
            
            if (ParticlePoints.Num() > 1)
            {
                try
                {
                    FSlateDrawElement::MakeLines(OutDrawElements, LayerId++, AllottedGeometry.ToPaintGeometry(), ParticlePoints, ESlateDrawEffect::None, ParticleColor, true, Size);
                    ParticleCount++;
                }
                catch (...) 
                {
                    UE_LOG(LogTemp, Error, TEXT("[HeartWidget] Failed to draw particle %d"), ParticleCount);
                }
            }
        }
        
        UE_LOG(LogTemp, Log, TEXT("[HeartWidget] DrawParticles finished, drew %d particles"), ParticleCount);
    }
    catch (const std::exception& e)
    {
        UE_LOG(LogTemp, Error, TEXT("[HeartWidget] Exception in DrawParticles: %s"), ANSI_TO_TCHAR(e.what()));
    }
    catch (...)
    {
        UE_LOG(LogTemp, Error, TEXT("[HeartWidget] Unknown exception in DrawParticles"));
    }
}