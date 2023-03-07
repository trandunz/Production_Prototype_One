// Fill out your copyright notice in the Description page of Project Settings.


#include "Fade.h"

Fade::Fade(UImage* _imageToFade, float _fadeTime)
{
	imageToFade = _imageToFade;
	fadeTime = _fadeTime;
	fadeTimer = fadeTime;
}

void Fade::Tick(float deltaTime)
{
	if (fadeTimer != fadeTime)
	{
		fadeTimer += deltaTime;
	}
	
	switch (fadeState)
	{
	case FadeState::FadeIn:
		fadeValue = fadeTime / fadeTimer;
		imageToFade->SetOpacity(fadeValue);
		break;
	case FadeState::FadeOut:
		fadeValue = fadeTime / fadeTimer;
		imageToFade->SetOpacity(1 - fadeValue);
		break;
	case FadeState::Halt:
		break;
	}
}

void Fade::FadeIn()
{
	fadeState = FadeState::FadeIn;
}

void Fade::FadeOut()
{
	fadeState = FadeState::FadeOut;
}

void Fade::StopFade()
{
	fadeState = FadeState::Halt;
}

void Fade::ImmediateFadeOut()
{
	fadeState = FadeState::Halt;
	fadeTimer = fadeTime;
	fadeValue = 0;
	imageToFade->SetOpacity(fadeValue);
}

void Fade::ImmediateFadeIn()
{
	fadeState = FadeState::Halt;
	fadeTimer = fadeTime;
	fadeValue = 1;
	imageToFade->SetOpacity(fadeValue);
}

Fade::~Fade()
{
}