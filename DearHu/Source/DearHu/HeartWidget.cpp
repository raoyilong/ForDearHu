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
    GenerateStaticParticles();
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
            float BeatScale = 1.0f + HeartBeatAmplitude * FMath::Sin(BeatTime * 3.14159f * 2.0f);
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
            
            // 绘制爱心路径（使用Lines），根据bShowHeartLines变量控制显示
            if (bShowHeartLines && ScaledPoints.Num() > 1)
            {
                FSlateDrawElement::MakeLines(OutDrawElements, CurrentLayerId++, AllottedGeometry.ToPaintGeometry(FVector2f(AllottedGeometry.GetLocalSize()), FSlateLayoutTransform()), ScaledPoints, ESlateDrawEffect::None, HeartColor, true, 2.0f);
            }
        }
        
        // 绘制粒子
        DrawParticles(OutDrawElements, CurrentLayerId, AllottedGeometry);
        
        // 绘制静态粒子
        DrawStaticParticles(OutDrawElements, CurrentLayerId, AllottedGeometry);
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
    FVector2D HeartCenter = AllottedGeometry.GetLocalSize() / 2.0f;
    
    // 安全检查：确保Particles数组有效
    if (Particles.Num() == 0)
    {
        return;
    }
    
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
        
        // 优化：简化粒子绘制，使用更高效的方式
        TArray<FVector2D> ParticlePoints;
        FVector2D Position = ParticlePosition - FVector2D(Size / 2.0f, Size / 2.0f);
        ParticlePoints.Add(Position);
        ParticlePoints.Add(Position + FVector2D(Size, Size));
        
        if (ParticlePoints.Num() > 1)
        {
            FSlateDrawElement::MakeLines(OutDrawElements, LayerId++, AllottedGeometry.ToPaintGeometry(), ParticlePoints, ESlateDrawEffect::None, ParticleColor, true, Size);
        }
    }
}

void UHeartWidget::GenerateStaticParticles()
{
    StaticParticles.Empty();
    
    // 确保StaticParticleCount为正数
    int32 SafeStaticParticleCount = FMath::Max(1, StaticParticleCount);
    
    // 从爱心边缘平均分布生成静态粒子
    if (HeartPoints.Num() > 0)
    {
        int32 Step = FMath::Max(1, HeartPoints.Num() / SafeStaticParticleCount);
        for (int32 i = 0; i < SafeStaticParticleCount; i++)
        {
            FStaticParticle Particle;
            
            // 平均分布选择爱心边缘的点
            int32 BaseIndex = (i * Step) % HeartPoints.Num();
            
            // 计算上一个点和下一个点的索引
            int32 PrevIndex = (BaseIndex - 1 + HeartPoints.Num()) % HeartPoints.Num();
            int32 NextIndex = (BaseIndex + 1) % HeartPoints.Num();
            
            // 获取上一个点和下一个点的位置
            FVector2D PrevPosition = HeartPoints[PrevIndex] * HeartSize;
            FVector2D NextPosition = HeartPoints[NextIndex] * HeartSize;
            
            // 在PrevPosition和NextPosition之间进行线性插值
            // StaticParticleRandomRange控制随机范围，0表示不随机，1表示在两个点之间完全随机
            float RandomFactor = FMath::FRandRange(-StaticParticleRandomRange, StaticParticleRandomRange);
            
            // 计算方向向量
            FVector2D Direction = (NextPosition - PrevPosition);
            float Distance = Direction.Size();
            if (Distance > 0)
            {
                Direction.Normalize();
                
                // 计算随机偏移
                float OffsetDistance = RandomFactor * (Distance / 2.0f);
                FVector2D RandomPosition = HeartPoints[BaseIndex] * HeartSize + Direction * OffsetDistance;
                
                Particle.Position = RandomPosition;
            }
            else
            {
                // 如果两点重合，使用基准点位置
                Particle.Position = HeartPoints[BaseIndex] * HeartSize;
            }
            
            Particle.Color = StaticParticleColor;
            Particle.Size = StaticParticleSize;
            StaticParticles.Add(Particle);
        }
    }
    else
    {
        // 如果没有爱心点，生成一个默认粒子
        FStaticParticle Particle;
        Particle.Position = FVector2D(0, 0);
        Particle.Color = StaticParticleColor;
        Particle.Size = StaticParticleSize;
        StaticParticles.Add(Particle);
    }
}

void UHeartWidget::DrawStaticParticles(FSlateWindowElementList& OutDrawElements, int32& LayerId, const FGeometry& AllottedGeometry) const
{
    try
    {
        FVector2D HeartCenter = AllottedGeometry.GetLocalSize() / 2.0f;
        float BeatScale = 1.0f + HeartBeatAmplitude * FMath::Sin(BeatTime * 3.14159f * 2.0f);
        
        // 安全检查：确保StaticParticles数组有效
        if (StaticParticles.Num() == 0)
        {
            return;
        }
        
        for (const FStaticParticle& Particle : StaticParticles)
        {
            // 计算粒子在Widget中的位置，并应用心跳缩放
            FVector2D ParticlePosition = Particle.Position * BeatScale + HeartCenter;
            
            // 确保位置有效
            if (ParticlePosition.X < -10000 || ParticlePosition.X > 10000 ||
                ParticlePosition.Y < -10000 || ParticlePosition.Y > 10000)
            {
                continue;
            }
            
            // 合并光晕效果和静态粒子，使静态粒子本身带有光晕效果
            FVector2D Center = ParticlePosition;
            
            // 计算最大半径（包含光晕）
            float MaxRadius = Particle.Size * StaticParticleGlowRadius;
            
            // 绘制合并后的粒子（从外到内）
            const int32 NumRings = 10; // 合并后的圆环数量
            for (int32 ring = 0; ring < NumRings; ring++)
            {
                // 计算当前圆环的半径，从最大（光晕外边缘）到最小（粒子中心）
                float RingSize = MaxRadius * (1.0f - (float)ring / (float)NumRings);
                
                // 计算当前圆环的透明度
                float Alpha;
                if (RingSize > Particle.Size)
                {
                    // 光晕部分：从外到内逐渐变亮
                    float GlowRatio = (RingSize - Particle.Size) / (MaxRadius - Particle.Size);
                    Alpha = 30.0f * (1.0f - GlowRatio);
                    if (Alpha < 5.0f) Alpha = 5.0f;
                }
                else
                {
                    // 粒子核心部分：保持较高透明度
                    float CoreRatio = RingSize / Particle.Size;
                    Alpha = 255.0f * CoreRatio;
                    if (Alpha < 100.0f) Alpha = 100.0f;
                }
                
                FColor RingColor = Particle.Color;
                RingColor.A = (uint8)Alpha;
                
                // 生成圆形点
                TArray<FVector2D> CirclePoints;
                const int32 NumCirclePoints = 12; // 圆形的边数
                for (int32 j = 0; j <= NumCirclePoints; j++)
                {
                    float Angle = (float)j / (float)NumCirclePoints * 3.14159f * 2.0f;
                    float X = Center.X + FMath::Cos(Angle) * RingSize / 2.0f;
                    float Y = Center.Y + FMath::Sin(Angle) * RingSize / 2.0f;
                    CirclePoints.Add(FVector2D(X, Y));
                }
                
                if (CirclePoints.Num() > 1)
                {
                    FSlateDrawElement::MakeLines(OutDrawElements, LayerId++, AllottedGeometry.ToPaintGeometry(), CirclePoints, ESlateDrawEffect::None, RingColor, true, 1.2f);
                }
            }
        }
    }
    catch (...)
    {
    }
}