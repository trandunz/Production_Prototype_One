#include "DialogueWidget.h"

#include "HttpModule.h"
#include "ShopWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Prototype_One/Characters/Prototype_OneCharacter.h"
#include "Prototype_One/Components/PlayerInventory.h"


FString UDialogueWidget::RandomHelloMessage{"Hi there traveller!"};

void UDialogueWidget::OnOpenAIResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful && Response->GetContentLength() > 0)
	{
		FString JsonResponse = Response->GetContentAsString();

		// Deserialize the JSON response into a struct
		FOpenAIResponse OpenAIResponse;
		TSharedPtr<FJsonObject> JsonObject;

		if (FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(JsonResponse), JsonObject))
		{
			// Deserialize the JSON object into the struct
			OpenAIResponse.id = JsonObject->GetStringField("id");

			TArray<TSharedPtr<FJsonValue>> ChoicesArray = JsonObject->GetArrayField("choices");
			for (const auto& ChoiceValue : ChoicesArray)
			{
				TSharedPtr<FJsonObject> ChoiceObject = ChoiceValue->AsObject();
				FString Text = ChoiceObject->GetStringField("text");
				OpenAIResponse.choices.Add(Text);
				UE_LOG(LogTemp, Warning, TEXT(" %s " ), *Text );
			}

			if (OpenAIResponse.choices.Num() > 0)
			{
				FString InputString = OpenAIResponse.choices[0];
				FString RegexString = "[A-Z][^\\.!?]*[\\.!?]";

				TArray<FString> Matches;
				ApplyRegex(InputString, RegexString, Matches);
				if (Matches.Num() > 0)
				{
					RandomHelloMessage = Matches[rand() % Matches.Num()];
					UE_LOG(LogTemp, Warning, TEXT(" %s " ), *RandomHelloMessage );
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to deserialize OpenAI JSON response: %s"), *JsonResponse);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to receive OpenAI response"));
	}
}

void UDialogueWidget::SendOpenAIRequest(const FString& _input)
{
	FString Url = "https://api.openai.com/v1/engines/text-davinci-003/completions";

	// Construct the request headers
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetVerb("POST");
	Request->SetURL(Url);
	Request->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *API_KEY));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	// Construct the request body
	FString RequestBody = "{\"prompt\":\"" + _input + """\""
							",\"max_tokens\":100"
							",""\"temperature\":0.2}";
	
	Request->SetContentAsString(RequestBody);

	// Send the request
	Request->OnProcessRequestComplete().BindStatic(&OnOpenAIResponseReceived);
	Request->ProcessRequest();
}

void UDialogueWidget::ApplyRegex(const FString& InputString, const FString& RegexString, TArray<FString>& OutMatches)
{
	// Compile the regex pattern
	FRegexPattern RegexPattern(RegexString);
	FRegexMatcher RegexMatcher(RegexPattern, InputString);

	// Loop through the matches and extract the matching sentences
	while (RegexMatcher.FindNext())
	{
		FString Match = RegexMatcher.GetCaptureGroup(0);
		OutMatches.Add(Match.TrimStartAndEnd());
	}
}

void UDialogueWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	Dialogue_NextLine->OnPressed.AddDynamic(this, &UDialogueWidget::OnNextDialogueLine);
	Bye->OnPressed.AddDynamic(this, &UDialogueWidget::OnBye);
	Quest->OnPressed.AddDynamic(this, &UDialogueWidget::OnQuestMenu);
	Shop->OnPressed.AddDynamic(this, &UDialogueWidget::OnShopMenu);
	ShopWidget->SetVisibility(ESlateVisibility::Hidden);
	
	RandomHelloMessage = DialogueLines[0];
	ShowDialogueBox(false);
	if (EnableAIParaphrasing)
		SendOpenAIRequest("Paraphrase: '" + RandomHelloMessage + "' with 5 results");

	
}

void UDialogueWidget::ShowDialogueBox(bool _show)
{
	if (_show)
	{
		DialogueLines = {};
		DialogueLines.Add("Hi there!");
		DialogueLines.Add("Can I help you with anything?");
		ShopWidget->SetVisibility(ESlateVisibility::Hidden);
		DialogueText->SetText(FText::FromString(RandomHelloMessage));
		Dialogue_NextLine->SetIsEnabled(true);
		Bye->SetIsEnabled(false);
		Bye->SetVisibility(ESlateVisibility::Hidden);
		Quest->SetIsEnabled(false);
		Quest->SetVisibility(ESlateVisibility::Hidden);
		Shop->SetIsEnabled(false);
		Shop->SetVisibility(ESlateVisibility::Hidden);
		SetVisibility(ESlateVisibility::Visible);
		CurrentDialogueIndex = 0;
		if (auto* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			playerController->SetInputMode(FInputModeUIOnly{});
			playerController->SetIgnoreMoveInput(true);
		}

		// Trigger audio in blueprint
		OnMoleAudioEvent();
	}
	else
	{
		if (EnableAIParaphrasing)
			SendOpenAIRequest("Paraphrase: '" + RandomHelloMessage + "' with 5 results");
		DialogueText->SetText(FText::FromString(RandomHelloMessage));
		SetVisibility(ESlateVisibility::Hidden);
		CurrentDialogueIndex = 0;
		if (auto* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			playerController->SetInputMode(FInputModeGameAndUI{});
			playerController->SetIgnoreMoveInput(false);
		}
	}
}

void UDialogueWidget::OnNextDialogueLine()
{
	CurrentDialogueIndex++;

	UE_LOG(LogTemp, Warning, TEXT("Next Dialogue Queue Please") );
	if (CurrentDialogueIndex >= DialogueLines.Num())
	{
		if (DialogueBackground->GetVisibility() == ESlateVisibility::Visible)
		{
			ShowDialogueBox(false);
		}
			
	}
	else if (CurrentDialogueIndex < DialogueLines.Num())
	{
		TArray<FString> matches{};
		ApplyRegex(DialogueLines[CurrentDialogueIndex], "\\?", matches);
		if (matches.Num() > 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("Regex Passed!") );
			Dialogue_NextLine->SetIsEnabled(false);
			Bye->SetIsEnabled(true);
			Bye->SetVisibility(ESlateVisibility::Visible);
			Quest->SetIsEnabled(true);
			Quest->SetVisibility(ESlateVisibility::Visible);
			Shop->SetIsEnabled(true);
			Shop->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			Dialogue_NextLine->SetIsEnabled(true);
			Bye->SetIsEnabled(false);
			Bye->SetVisibility(ESlateVisibility::Hidden);
			Quest->SetIsEnabled(false);
			Quest->SetVisibility(ESlateVisibility::Hidden);
			Shop->SetIsEnabled(false);
			Shop->SetVisibility(ESlateVisibility::Hidden);
		}
		DialogueText->SetText(FText::FromString(DialogueLines[CurrentDialogueIndex]));
	}
	// Trigger audio in blueprint
	OnMoleAudioEvent();	
}

void UDialogueWidget::OnBye()
{
	DialogueLines = {};
	CurrentDialogueIndex = -1;
	DialogueLines.Add("Thank you, come again.");
	OnNextDialogueLine();
	
	// Trigger audio in blueprint
	OnMoleAudioEvent();

	// Set the PlayerInventory to shopping so player can sell while at shop only
    if (auto* player = Cast<APrototype_OneCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
    {
    	player->PlayerInventory->SetIsShopping(false);
    	if (auto* charatcer = Cast<APrototype_OneCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
    	{
    		charatcer->IsShopping = false;
    	}
    }
    else
    {
    	UKismetSystemLibrary::PrintString(GetWorld(),"Cast to Prototype_OneCharacter failed in UDialogueWidget::OnShopMenu()");
    }
}


void UDialogueWidget::OnQuestMenu()
{
	if (DialogueLines.Num() <= 2)
	{
		DialogueLines.Add("If you are looking to enter Mole Metropolis,");
		DialogueLines.Add("you will need to purchase an expensive ticket.");
		DialogueLines.Add("Search the woods for items that I will buy from you.");
		DialogueLines.Add("But beware, carrots will attract the Ruthless Rabbits,");
		DialogueLines.Add("but they in turn will provide you with high valuable items.");
		DialogueLines.Add("Also, keep an eye out for the Ruler Rabbit.");
		DialogueLines.Add("I will buy his crown from you for a large sum.");
	}
	
	OnNextDialogueLine();
}

void UDialogueWidget::OnShopMenu()
{
	ShopWidget->SetVisibility(ESlateVisibility::Visible);

	// Set the PlayerInventory to shopping so player can sell while at shop only
	if (auto* player = Cast<APrototype_OneCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		player->PlayerInventory->SetIsShopping(true);
	}
	else
	{
		UKismetSystemLibrary::PrintString(GetWorld(),"Cast to Prototype_OneCharacter failed in UDialogueWidget::OnShopMenu()");
	}

	// Trigger audio in blueprint
	OnMoleAudioEvent();
}
