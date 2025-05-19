import pytest
from httpx import AsyncClient
from app.main import app

@pytest.mark.asyncio
async def test_register_and_login():
    async with AsyncClient(app=app, base_url="http://test") as ac:
        res = await ac.post("/register", json={"email": "test@example.com", "password": "secret"})
        assert res.status_code == 200
        res = await ac.post("/login", data={"username": "test@example.com", "password": "secret"})
        assert res.status_code == 200
        assert "access_token" in res.json()