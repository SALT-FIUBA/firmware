#!/bin/bash

BRIDGE_IF="bridge100"

echo "🧨 Tearing down $BRIDGE_IF..."

if ifconfig "$BRIDGE_IF" >/dev/null 2>&1; then
  sudo ifconfig "$BRIDGE_IF" destroy
  echo "✅ Bridge removed."
else
  echo "⚠️ $BRIDGE_IF not found."
fi

