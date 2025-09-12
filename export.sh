#!/bin/bash
set -euo pipefail

HARDWARE_DIR="./Hardware"
DOCS_DIR="./Docs"
TEMP_DIR="$DOCS_DIR/temp"
TEMP_SCH_DIR="$DOCS_DIR/temp_sch"

PCB_FILE=$(ls "$HARDWARE_DIR"/*.kicad_pcb 2>/dev/null | head -n1 || true)
if [ -z "$PCB_FILE" ]; then
    echo "Keine .kicad_pcb Datei gefunden!"
    exit 1
fi

PROJECT_NAME=$(basename "$PCB_FILE" .kicad_pcb)

mkdir -p "$TEMP_DIR"
LAYERS=("F.Cu" "In1.Cu" "In2.Cu" "B.Cu" "F.SilkS" "B.SilkS" "F.Mask" "B.Mask" "Edge.Cuts")
EXPORT_PDFS=()

echo "Exportiere PCB-Layer..."
for layer in "${LAYERS[@]}"; do
    OUT_FILE="$TEMP_DIR/board_${layer//./_}.pdf"
    if grep -q "$layer" "$PCB_FILE"; then
        echo "Exportiere Layer: $layer -> $OUT_FILE"
        kicad-cli pcb export pdf --output "$OUT_FILE" --layers "$layer" "$PCB_FILE"
        if [ -f "$OUT_FILE" ]; then
            EXPORT_PDFS+=("$OUT_FILE")
        else
            echo "Fehler: PDF für Layer $layer wurde nicht erstellt!"
        fi
    else
        echo "Layer $layer nicht vorhanden – überspringe."
    fi
done

if [ ${#EXPORT_PDFS[@]} -gt 0 ]; then
    echo "Kombiniere PDF-Dateien..."
    mkdir -p "$DOCS_DIR"
    pdfunite "${EXPORT_PDFS[@]}" "$DOCS_DIR/${PROJECT_NAME}_board_layers.pdf"
else
    echo "Keine Layer-PDFs vorhanden. Export abgebrochen."
    exit 1
fi

# Export Schaltplan
mkdir -p "$TEMP_SCH_DIR"
SCH_FILE="$HARDWARE_DIR/${PROJECT_NAME}.kicad_sch"
if [ -f "$SCH_FILE" ]; then
    echo "Exportiere Schaltplan -> $DOCS_DIR/${PROJECT_NAME}_schematics.pdf"
    kicad-cli sch export pdf --output "$DOCS_DIR/${PROJECT_NAME}_schematics.pdf" "$SCH_FILE"
else
    echo "Keine passende .kicad_sch-Datei gefunden"
fi

# Render PCB-Vorschau
TIMESTAMP=$(date +%Y-%m-%d)
SHORT_HASH=$(git rev-parse --short HEAD || echo "nogit")
PREVIEW_FILE_TOP="$DOCS_DIR/board_preview_top_${TIMESTAMP}_${SHORT_HASH}.png"
PREVIEW_FILE_BOTTOM="$DOCS_DIR/board_preview_bottom_${TIMESTAMP}_${SHORT_HASH}.png"

echo "Erstelle PCB-Vorschau -> $PREVIEW_FILE_TOP"
kicad-cli pcb render \
    --output "$PREVIEW_FILE_TOP" \
    --width 1920 \
    --height 1080 \
    --side top \
    --background transparent \
    --quality high \
    "$PCB_FILE"

echo "Erstelle PCB-Vorschau -> $PREVIEW_FILE_BOTTOM"
kicad-cli pcb render \
    --output "$PREVIEW_FILE_BOTTOM" \
    --width 1920 \
    --height 1080 \
    --side bottom \
    --background transparent \
    --quality high \
    "$PCB_FILE"


echo "Export abgeschlossen!"

echo "Füge Bilder in README.md ein..."
if [ -f README.md ]; then
  sed -i "/<!-- Keep this line! Rendered picture of PCB is going to be displayed after Production files are pushed to branch! -->/a ![]($PREVIEW_FILE_BOTTOM)" README.md
  sed -i "/<!-- Keep this line! Rendered picture of PCB is going to be displayed after Production files are pushed to branch! -->/a ![]($PREVIEW_FILE_TOP)" README.md
fi


# Aufräumen: temporäre Verzeichnisse löschen
rm -rf "$TEMP_DIR" "$TEMP_SCH_DIR"
echo "Temporäre Verzeichnisse gelöscht."
