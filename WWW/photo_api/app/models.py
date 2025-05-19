from sqlalchemy import Column, Integer, String, ForeignKey, DateTime, UniqueConstraint
from sqlalchemy.orm import relationship
from datetime import datetime
from app.database import Base

class User(Base):
    __tablename__ = "users"

    id = Column(Integer, primary_key=True, index=True)
    email = Column(String, unique=True, index=True)
    hashed_password = Column(String)
    role = Column(String, default="user")
    photos = relationship("Photo", back_populates="owner")
    likes = relationship("Like", back_populates="user")

class Photo(Base):
    __tablename__ = "photos"

    id = Column(Integer, primary_key=True, index=True)
    url = Column(String)
    description = Column(String)
    owner_id = Column(Integer, ForeignKey("users.id"))
    created_at = Column(DateTime, default=datetime.utcnow)
    owner = relationship("User", back_populates="photos")
    likes = relationship("Like", back_populates="photo")

class Like(Base):
    __tablename__ = "likes"
    __table_args__ = (UniqueConstraint('user_id', 'photo_id', name='unique_like'),)

    id = Column(Integer, primary_key=True, index=True)
    user_id = Column(Integer, ForeignKey("users.id"))
    photo_id = Column(Integer, ForeignKey("photos.id"))
    user = relationship("User", back_populates="likes")
    photo = relationship("Photo", back_populates="likes")
