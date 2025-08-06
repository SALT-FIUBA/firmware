#!/bin/bash

# Run the script
# ./get_ca_cert.sh https://google.com 2

# certificate index number:
# 2 -> intermediate CA
# 3 -> root CA


# Check if URL is provided
if [ -z "$1" ]; then
    echo "Usage: $0 <URL> [cert_index]"
    echo "Example: $0 https://google.com 2"
    exit 1
fi

# Extract hostname from URL
URL="$1"
CERT_INDEX="${2:-2}" # Default to second certificate if not specified
HOSTNAME=$(echo "$URL" | awk -F/ '{print $3}' | awk -F: '{print $1}')
if [ -z "$HOSTNAME" ]; then
    echo "Error: Could not extract hostname from URL '$URL'. Ensure URL is valid."
    exit 1
fi

OUTPUT_PEM="ca-cert.pem"
OUTPUT_HEADER="ca-cert.h"

# Step 1: Retrieve the certificate chain
echo "Fetching certificate chain for $HOSTNAME..."
openssl s_client -showcerts -connect "$HOSTNAME":443 < /dev/null > certs.pem 2>/dev/null
if [ $? -ne 0 ]; then
    echo "Error: Failed to retrieve certificates for $HOSTNAME. Check URL or network."
    rm -f certs.pem
    exit 1
fi

# Step 2: Extract the specified CA certificate
echo "Extracting certificate #$CERT_INDEX..."
awk '/-----BEGIN CERTIFICATE-----/{n++} n=='"$CERT_INDEX"'{print}' certs.pem > "$OUTPUT_PEM"
if [ ! -s "$OUTPUT_PEM" ]; then
    echo "Error: Failed to extract certificate #$CERT_INDEX. Check if index is valid."
    rm -f certs.pem "$OUTPUT_PEM"
    exit 1
fi

# Step 3: Convert PEM to C array
echo "Converting certificate to C header file..."
xxd -i "$OUTPUT_PEM" > "$OUTPUT_HEADER"
if [ $? -ne 0 ]; then
    echo "Error: Failed to convert certificate to C array. Ensure xxd is installed."
    rm -f certs.pem "$OUTPUT_PEM" "$OUTPUT_HEADER"
    exit 1
fi

# Step 4: Rename variables in the header file
echo "Renaming variables in $OUTPUT_HEADER..."
sed -i '' 's/ca_cert_pem/ca_cert/g' "$OUTPUT_HEADER"
sed -i '' 's/ca_cert_pem_len/ca_cert_len/g' "$OUTPUT_HEADER"
if [ $? -ne 0 ]; then
    echo "Error: Failed to rename variables in $OUTPUT_HEADER."
    rm -f certs.pem "$OUTPUT_PEM" "$OUTPUT_HEADER"
    exit 1
fi

# Clean up temporary files
rm -f certs.pem "$OUTPUT_PEM"

echo "Success: CA certificate saved to $OUTPUT_HEADER"
echo "Verify the certificate with: openssl x509 -in $OUTPUT_PEM -text -noout"
