from pydantic import BaseModel, EmailStr
from typing import Optional, List
from datetime import datetime

class UserCreate(BaseModel):
    email: EmailStr
    password: str

class UserOut(BaseModel):
    id: int
    email: EmailStr
    role: str
    class Config:
        orm_mode = True

class Token(BaseModel):
    access_token: str
    token_type: str

class PhotoCreate(BaseModel):
    url: str
    description: Optional[str] = None

class PhotoOut(BaseModel):
    id: int
    url: str
    description: Optional[str]
    created_at: datetime
    owner: UserOut
    class Config:
        orm_mode = True

class LikeCreate(BaseModel):
    photo_id: int

class LikeOut(BaseModel):
    id: int
    user: UserOut
    photo: PhotoOut
    class Config:
        orm_mode = True