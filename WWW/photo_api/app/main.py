from fastapi import FastAPI
from app.routers import users, photos, likes
from app.database import Base, engine

Base.metadata.create_all(bind=engine)

app = FastAPI(title="Photo Sharing API")

app.include_router(users.router)
app.include_router(photos.router)
app.include_router(likes.router)