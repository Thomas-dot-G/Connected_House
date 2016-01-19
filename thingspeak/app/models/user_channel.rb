# == Schema Information
#
# Table name: channels
#
#  id                        :integer          not null, primary key
#  user_id                   :integer
#  name                      :string(255)
#  feeds                     :feed[]
#  description               :string(255)




class User_Channel < ActiveRecord::Base
include KeyUtilities
belongs_to :user
has_many :feeds
has_many :api_keys, :dependent => :destroy
scope :public_viewable, lambda { where("public_flag = true") }
scope :is_public, lambda { where("public_flag = true") }

