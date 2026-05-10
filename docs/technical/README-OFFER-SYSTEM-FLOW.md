# Offer System Flow

This document explains how offer events flow through No Limit Connect GUI and engine layers.

## Purpose

- Use this document when adding new offer features.
- Use this document when debugging missing Offer List entries, title-bar offer notify dots, or ring behavior.

## Core Components

- GuiOfferMgr and GuiOfferMgrBase hold GUI offer sessions shown in Offer List.
- GuiOfferSession wraps OfferBaseInfo with GUI-only state.
- TitleBarWidget listens through GuiOfferCallback and updates offer notify/count UI.
- OfferMgr handles normal plugin-offer transport and callbacks.
- RandomConnectMgr and GuiRandConnectMgr handle random-connect relay actions.

## Standard Plugin Offer Flow

- Send path uses GuiOfferMgrBase::fromGuiMakePluginOffer.
- Receive path uses GuiOfferMgrBase::toGuiRxedPluginOffer.
- Reply path uses GuiOfferMgrBase::toGuiRxedOfferReply.
- Receive state transitions to eOfferStateRxedByUser.
- Reply states map from EOfferResponse to GUI offer states.
- Terminal states stop ring and move/remove entries from active offer list.

## Offer List and Title-Bar Notify

- Offer List and title-bar notify are driven by GuiOfferMgr callbacks.
- callbackActiveOfferCount(activeCnt, historyCnt) controls notify state and active count label.
- If an event does not pass through GuiOfferMgr, Offer List and title-bar notify will not update.

## Ring Behavior

- Ring starts for phone-type plugins when state becomes eOfferStateRxedByUser.
- Ring stops on terminal and session states such as accepted, rejected, canceled, busy, offline, no response, missed, and session complete.

## Random Connect Integration

- Random Connect offers are relay actions (ERandAction), not normal plugin-offer network packets.
- GuiRandConnectMgr bridges random-connect lifecycle events into GuiOfferMgr.
- OfferRequest is mirrored into GuiOfferMgr for local participants.
- Sender-side OfferRequest is mirrored so outgoing random-connect offers appear in Offer List flow.
- Terminal random-connect actions are mapped to EOfferResponse and mirrored into GuiOfferMgr reply handling.

## Random Connect Action Mapping

- eRandActionOfferAccept maps to eOfferResponseAccept.
- eRandActionOfferReject maps to eOfferResponseReject.
- eRandActionOfferCancel maps to eOfferResponseCancelSession.
- eRandActionOfferNoResponse maps to eOfferResponseEndSession.
- eRandActionOfferMissed maps to eOfferResponseEndSession.

## Debug Checklist

- Confirm event bridge reached GuiOfferMgr via toGuiRxedPluginOffer or toGuiRxedOfferReply.
- Confirm OfferBaseInfo has valid offerId, pluginType, creatorId, and historyId.
- Confirm expected GuiOfferSession state transitions occur.
- Confirm callbackActiveOfferCount is firing.
- Confirm local-role mapping is correct for incoming target versus sender-side mirror.

## Developer Notes

- Keep random-connect transport details in random-connect-focused docs.
- Keep user-visible offer behavior unified through GuiOfferMgr pipeline.
- Prefer bridges/adapters over duplicating offer-state logic in multiple managers.
