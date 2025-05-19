#!/bin/bash

BASE_URL="http://localhost:3001"
EMAIL="test@example.com"
PASSWORD="test"

# Logowanie i pobranie tokena
echo "Logowanie"
TOKEN=$(curl -s -X POST "$BASE_URL/login" \
  -H "Content-Type: application/json" \
  -d "{\"email\":\"$EMAIL\", \"password\":\"$PASSWORD\"}" | jq -r '.token')

if [ "$TOKEN" = "null" ] || [ -z "$TOKEN" ]; then
  echo "Logowanie nieudane. Sprawdź dane logowania."
  exit 1
fi

echo "Zalogowano. Token JWT: $TOKEN"

# Test 1: Dodanie książki
echo "Dodawanie książki"
curl -s -X POST "$BASE_URL/books/add" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d '{"title":"1984", "author":"George Orwell","isbn":"978-83-240-1137-7","publishedYear":"1949","genre":"Dystopia"}' | jq

# Test 2: Pobranie listy książek
echo "Pobieranie listy książek"
curl -s -X GET "$BASE_URL/books" \
  -H "Authorization: Bearer $TOKEN" | jq

# Test 3: Dodanie użytkownika
echo "Rejestracja użytkownika"
curl -s -X POST "$BASE_URL/register" \
  -H "Content-Type: application/json" \
  -d '{"email":"testuser@example.com", "password":"test123"}' | jq

# Test 4: Pobranie listy użytkowników
echo "Lista użytkowników"
curl -s -X GET "$BASE_URL/users" \
  -H "Authorization: Bearer $TOKEN" | jq

# Test 5: Usunięcie użytkownika (przykład ID = )
echo "Usuwanie użytkownika o ID 16"
curl -s -X DELETE "$BASE_URL/users/16/delete" \
  -H "Authorization: Bearer $TOKEN" | jq

# Test 6: Pobranie listy użytkowników
echo "Lista użytkowników"
curl -s -X GET "$BASE_URL/users" \
  -H "Authorization: Bearer $TOKEN" | jq
