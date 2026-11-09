---
name: make-decision
description: Classify, route, or score short text with the local tev1 decision model via the make_decision tool (Ollama /v1/systemone)
---

## Decision Model (`make_decision`)

Use `make_decision(state, questions)` when a task reduces to a fast classification, routing choice, policy check, or rubric score over short text — e.g. "which module owns this error?", "is this request within policy?", "rate the severity". It calls a local decision model (tev1) that returns the chosen option with probabilities.

### Prerequisites

- `decision.enabled` must be `true` in `.opencode/opencode-rag.json` (off by default).
- Requires Ollama >= 0.35 with a pulled tev1 model (`ollama pull tev1` or `ollama pull tev1:0.8b`).
- Keep `state` short — the effective context is ~2k tokens. Pass the relevant excerpt, not a whole file.

### Questions

Each question needs `id`, `type`, and `instructions`:

- `choice` — pick one of 2-24 options. `criteria` maps option → description. Add a `none` option when no listed option may fit.
- `noul` — true/false probability. Optional criteria `{ "true": "...", "false": "..." }` describe each side.
- `score` — place the state on a rubric. `criteria` is an ordered array of 2-24 level descriptions, lowest first.

### Example

```json
{
  "state": "Customer says they were charged twice for the October subscription.",
  "questions": [
    {
      "id": "intent",
      "type": "choice",
      "instructions": "Which support intent best matches the message?",
      "criteria": {
        "duplicate_charge": "Charged more than once.",
        "cancel": "Wants to end the subscription.",
        "none": "None of the listed intents matches."
      }
    }
  ]
}
```

### Optional: route decisions before asking the user

- When `decision.routeBeforeAsking` is `true`, route option choices through `make_decision` before interrupting the user: proceed when the model is confident, and ask the user only when it is undecided (low `confidence`) or the choice depends on personal preference.

### Limits

- 1-64 questions per call; 2-24 options/levels per question.
- `confidence` is probability concentration, NOT the chance the answer is correct.
- Never rely on it as the only check for a high-stakes decision; treat low-confidence answers as undecided.
